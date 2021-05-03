#include "OgreMainWindow.h"

#include <OgreHeaderPrefix.h>

#include <Bites/OgreBitesConfigDialog.h>

#include <OgreCamera.h>
#include <OgreRenderWindow.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreString.h>
#include <OgreViewport.h>

#include <OgreHeaderSuffix.h>

#include <CEGUI/RendererModules/Ogre/ImageCodec.h>
#include <CEGUI/RendererModules/Ogre/ResourceProvider.h>

OgreMainWindow::OgreMainWindow() {
#ifdef DEBUG
    Ogre::String pluginsFileName = "plugins_d.cfg";
#else
    Ogre::String pluginsFileName = "plugins.cfg";
#endif

    d_ogreRoot = new Ogre::Root(pluginsFileName);

    if (d_ogreRoot->showConfigDialog(OgreBites::getNativeConfigDialog())) {
        // initialise system according to user options.
        d_window = d_ogreRoot->initialise(true);

        // Create the scene manager
        Ogre::SceneManager* sm = d_ogreRoot->createSceneManager(Ogre::ST_GENERIC, "SampleSceneMgr");
        // Create and initialise the camera
        d_camera = sm->createCamera("SampleCam");
        d_camera->setPosition(Ogre::Vector3(0, 0, 500));
        d_camera->lookAt(Ogre::Vector3(0, 0, -300));
        d_camera->setNearClipDistance(5);

        // Create a viewport covering whole window
        Ogre::Viewport* vp = d_window->addViewport(d_camera);
        vp->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f));
        // Update the camera aspect ratio to that of the viewport
        d_camera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

        // create ogre renderer, image codec and resource provider.
        CEGUI::OgreRenderer& renderer = CEGUI::OgreRenderer::create();
        d_renderer = &renderer;
        d_imageCodec = &renderer.createOgreImageCodec();
        d_resourceProvider = &renderer.createOgreResourceProvider();

        // create frame listener
        // d_frameListener = new CEGuiDemoFrameListener(this, d_sampleApp, d_window, d_camera);
        // d_ogreRoot->addFrameListener(d_frameListener);

        // add a listener for OS framework window events (for resizing)
        // d_windowEventListener = new WndEvtListener(d_frameListener->getOISMouse());
        // WindowEventUtilities::addWindowEventListener(d_window, d_windowEventListener);

        // d_ogreRoot->addFrameListener(this);
        renderer.setRenderingEnabled(false);

        // d_initialised = true;
    } else {
        // aborted.  Clean up and set root to 0 so when app attempts to run it
        // knows what happened here.
        delete d_ogreRoot;
        d_ogreRoot = 0;
    }
}

OgreMainWindow::~OgreMainWindow() { delete d_ogreRoot; }
