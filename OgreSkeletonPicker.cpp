#include <SkeletonPicker.h>

#include <OgreHeaderPrefix.h>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreMesh.h>
#include <OgreMovableObject.h>
#include <OgreNode.h>
#include <OgreRay.h>
#include <OgreSceneManager.h>
#include <OgreSubMesh.h>

#include <OgreHeaderSuffix.h>

namespace {

// Get the mesh information for the given mesh.
// Code found in Wiki: www.ogre3d.org/wiki/index.php/RetrieveVertexData
void GetMeshInformation(const Ogre::MeshPtr mesh, size_t& vertex_count, Ogre::Vector3*& vertices, size_t& index_count,
                        unsigned long*& indices, const Ogre::Vector3& position, const Ogre::Quaternion& orient,
                        const Ogre::Vector3& scale) {
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i) {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        // We only need to add the shared vertices once
        if (submesh->useSharedVertices) {
            if (!added_shared) {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        } else {
            vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        index_count += submesh->indexData->indexCount;
    }

    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i) {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared)) {
            if (submesh->useSharedVertices) {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //      Ogre::Real* pReal;
            float* pReal;

            for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize()) {
                posElem->baseVertexPointerToElement(vertex, &pReal);

                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
        if (ibuf.isNull())
            continue; // need to check if index buffer is valid (which will be not if the mesh doesn't have triangles
                      // like a pointcloud)

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;
        size_t index_start = index_data->indexStart;
        size_t last_index = numTris * 3 + index_start;

        if (use32bitindexes)
            for (size_t k = index_start; k < last_index; ++k) {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }

        else
            for (size_t k = index_start; k < last_index; ++k) {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) + static_cast<unsigned long>(offset);
            }

        ibuf->unlock();
        current_offset = next_offset;
    }
}

static float CalculateClosestDistanceToMesh(const Ogre::Entity& entity, const Ogre::Ray& pickRay) {
    // mesh data to retrieve
    size_t vertex_count;
    size_t index_count;
    Ogre::Vector3* vertices;
    unsigned long* indices;

    GetMeshInformation(entity.getMesh(), vertex_count, vertices, index_count, indices,
                       entity.getParentNode()->getPosition(), entity.getParentNode()->getOrientation(),
                       entity.getParentNode()->_getDerivedScale());

    // test for hitting individual triangles on the mesh
    bool new_closest_found = false;
    Ogre::Real closest_distance = -1.0f;
    for (int i = 0; i < static_cast<int>(index_count); i += 3) {
        // check for a hit against this triangle
        std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(
            pickRay, vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]], true, false);

        // if it was a hit check if its the closest
        if (hit.first) {
            if ((closest_distance < 0.0f) || (hit.second < closest_distance)) {
                // this is the closest so far, save it off
                closest_distance = hit.second;
                new_closest_found = true;
            }
        }
    }

    // free the verticies and indicies memory
    delete[] vertices;
    delete[] indices;

    return closest_distance;
}

static Ogre::MovableObject* QueryMovableObjectUsingBBox(Ogre::SceneManager& sceneManager, const Ogre::Ray& pickRay) {
    auto* query = sceneManager.createRayQuery(Ogre::Ray());
    query->setRay(pickRay);
    query->setSortByDistance(true /*sort by depth*/, 1 /*only one result needed*/);
    auto& results = query->execute();

    if (results.empty()) {
        sceneManager.destroyQuery(query);
        return nullptr;
    }
    auto* movable = results.front().movable;
    sceneManager.destroyQuery(query);
    return movable;
}

} // namespace

namespace SkeletonPicker {

CJoint* Pick(PickContext& context, float x, float y) {
    const auto pickRay = context.camera.getCameraToViewportRay(x, y);
    auto* pickedMovable = QueryMovableObjectUsingBBox(context.sceneManager, pickRay);

    if (pickedMovable == nullptr)
        return nullptr;

    auto jointIt = context.backwardsMapping.find(pickedMovable);

    if (jointIt == context.backwardsMapping.end())
        return nullptr; // The picked entity is not a skeleton joint

    if (auto* entity = dynamic_cast<const Ogre::Entity*>(jointIt->first)) {
        const float closest_distance = CalculateClosestDistanceToMesh(*entity, pickRay);
        // return the result
        if (closest_distance >= 0.0f) {
            // raycast success
            return jointIt->second;
        } else {
            // raycast failed
            return nullptr;
        }
    }

    // The picked MovableObject was in the backwards mapping but it was not an Entity, weird
    return nullptr;
}
} // namespace SkeletonPicker