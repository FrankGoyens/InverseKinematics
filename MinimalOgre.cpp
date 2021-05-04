#include "MinimalOgre.h"

#include <OgreHeaderPrefix.h>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreMaterialManager.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include <Bites/OgreCameraMan.h>
#include <Bites/OgreTrays.h>

#include <OgreHeaderSuffix.h>

MinimalOgre::MinimalOgre() : OgreBites::ApplicationContext("OgreTutorialApp") {}

MinimalOgre::~MinimalOgre() { delete m_cameraMan; }

void MinimalOgre::loadResources() {
    enableShaderCache();
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Essential");

    createDummyScene();

    m_trays = new OgreBites::TrayManager("Interface", getRenderWindow());
    addInputListener(m_trays);

    m_trays->showLoadingBar(1, 0);
    OgreBites::ApplicationContext::loadResources();

    m_trays->hideLoadingBar();
    destroyDummyScene();
}

void MinimalOgre::setup() { // do not forget to call the base first
    OgreBites::ApplicationContext::setup();

    // register for input events
    addInputListener(this);

    // get a pointer to the already created root
    Ogre::Root* root = getRoot();
    Ogre::SceneManager* scnMgr = root->createSceneManager();

    // register our scene with the RTSS
    Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    // Trays
    scnMgr->addRenderQueueListener(getOverlaySystem());
    m_trays->showFrameStats(OgreBites::TL_TOPRIGHT);
    m_trays->refreshCursor();

    auto* rs = shadergen->getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    rs->addTemplateSubRenderState(shadergen->createSubRenderState(Ogre::RTShader::PerPixelLighting().getType()));

    scnMgr->setAmbientLight(Ogre::ColourValue(.5, .5, .5));

    // without light we would just get a black screen
    Ogre::Light* light = scnMgr->createLight("MainLight");
    Ogre::SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    lightNode->setPosition(20, 80, 50);
    lightNode->attachObject(light);

    // also need to tell where we are
    Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    camNode->setPosition(0, 0, 15);
    camNode->lookAt(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_PARENT);

    // create the camera
    Ogre::Camera* cam = scnMgr->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);

    // and tell it to render into the main window
    getRenderWindow()->addViewport(cam);

    // finally something to render
    Ogre::Entity* ent = scnMgr->createEntity("sphere.mesh");
    auto material = Ogre::MaterialManager::getSingleton().getByName("Template/Blue");
    ent->setMaterial(material);
    Ogre::SceneNode* node = scnMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(ent);
    node->scale({.01f, .01f, .01f});

    // Render a line
    auto* manualObject_line = scnMgr->createManualObject();
    auto lineMaterial = Ogre::MaterialManager::getSingleton().getByName("Template/GreenNonShaded");

    manualObject_line->begin(lineMaterial, Ogre::RenderOperation::OT_LINE_STRIP);
    manualObject_line->position({0, 0, 0});
    manualObject_line->position({10, 0, 0});
    manualObject_line->end();

    auto* lineNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    lineNode->attachObject(manualObject_line);

    // Set up the cameraman
    m_cameraMan = new OgreBites::CameraMan(camNode);
    m_cameraMan->setTarget(node);
    m_cameraMan->setStyle(OgreBites::CameraStyle::CS_FREELOOK);
    m_cameraMan->setYawPitchDist(Ogre::Radian(0), Ogre::Radian(0.3f), 50);

    addInputListener(m_cameraMan);
}

void MinimalOgre::shutdown() {
    delete m_trays;
    OgreBites::ApplicationContextBase::shutdown();
}

bool MinimalOgre::keyPressed(const OgreBites::KeyboardEvent& evt) {
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE) {
        getRoot()->queueEndRendering();
    }
    return true;
}
