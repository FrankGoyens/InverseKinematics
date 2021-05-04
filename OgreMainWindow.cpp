#include "OgreMainWindow.h"

#include <OIS.h>

#include <OgreHeaderPrefix.h>

#include <Bites/OgreBitesConfigDialog.h>
#include <Bites/OgreWindowEventUtilities.h>

#include <OgreCamera.h>
#include <OgreFrameListener.h>
#include <OgreRenderWindow.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreString.h>
#include <OgreViewport.h>

#include <OgreHeaderSuffix.h>

#include <CEGUI/CEGUI.h>

#include <CEGUI/GUIContext.h>
#include <CEGUI/MouseCursor.h>
#include <CEGUI/RendererModules/Ogre/ImageCodec.h>
#include <CEGUI/RendererModules/Ogre/ResourceProvider.h>
#include <CEGUI/System.h>

////////////////
///// WndEvtListener declaration
////////////////

class WndEvtListener : public OgreBites::WindowEventListener {
  public:
    WndEvtListener(OIS::Mouse* mouse);

    void windowResized(Ogre::RenderWindow* rw);

  protected:
    OIS::Mouse* d_mouse;
};

////////////////
///// Frame listener declaration
////////////////

class InverseKinematicsFrameListener : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener {
  public:
    // Construction and Destruction
    InverseKinematicsFrameListener(OgreMainWindow* baseApp, Ogre::RenderWindow* win, Ogre::Camera* cam);
    ~InverseKinematicsFrameListener();

    // Processing to be done at start and end of each frame.
    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);

    // Raw input handlers that we care about
    bool mouseMoved(const OIS::MouseEvent& e);
    bool keyPressed(const OIS::KeyEvent& e);
    bool keyReleased(const OIS::KeyEvent& e);
    bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id);

    OIS::Mouse* getOISMouse();

  protected:
    CEGUI::MouseButton convertOISButtonToCegui(int buttonID);

    OIS::InputManager* d_inputManager;
    OIS::Keyboard* d_keyboard;
    OIS::Mouse* d_mouse;
    Ogre::Camera* d_camera;
    Ogre::RenderWindow* d_window;

    OgreMainWindow* d_baseApp;
};

////////////////
///// Ogre main window impl
////////////////

OgreMainWindow::OgreMainWindow() {
#ifdef DEBUG
    Ogre::String pluginsFileName = "plugins_d.cfg";
#else
    Ogre::String pluginsFileName = "plugins.cfg";
#endif

    d_ogreRoot = std::make_unique<Ogre::Root>(pluginsFileName);

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

        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        // create ogre renderer, image codec and resource provider.
        CEGUI::OgreRenderer& renderer = CEGUI::OgreRenderer::bootstrapSystem();

        CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
        CEGUI::Font::setDefaultResourceGroup("fonts");
        CEGUI::Scheme::setDefaultResourceGroup("schemes");
        CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeel");
        CEGUI::WindowManager::setDefaultResourceGroup("layouts");

        CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
        CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

        // create frame listener
        m_frameListener = std::make_unique<InverseKinematicsFrameListener>(this, d_window, d_camera);
        d_ogreRoot->addFrameListener(m_frameListener.get());

        // add a listener for OS framework window events (for resizing)
        d_windowEventListener = std::make_unique<WndEvtListener>(m_frameListener->getOISMouse());
        OgreBites::WindowEventUtilities::addWindowEventListener(d_window, d_windowEventListener.get());

        d_ogreRoot->addFrameListener(this);
        renderer.setRenderingEnabled(false);

        MakeGUI();

    } else {
        // aborted.  Clean up and set root to 0 so when app attempts to run it
        // knows what happened here.
        d_ogreRoot.reset();
    }
}

OgreMainWindow::~OgreMainWindow() { CEGUI::System::destroy(); }

bool OgreMainWindow::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    CEGUI::System& gui_system(CEGUI::System::getSingleton());

    CEGUI::Renderer* gui_renderer(gui_system.getRenderer());
    gui_renderer->beginRendering();

    gui_system.getDefaultGUIContext().draw();

    gui_renderer->endRendering();
    CEGUI::WindowManager::getSingleton().cleanDeadPool();

    gui_system.injectTimePulse(evt.timeSinceLastFrame);

    return true;
};

void OgreMainWindow::Run() { d_ogreRoot->startRendering(); }

void OgreMainWindow::MakeGUI() {
    CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window* sheet = wmgr.createWindow("DefaultWindow", "Root");
    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);
}

////////////////
///// Frame listener impl
////////////////

InverseKinematicsFrameListener::InverseKinematicsFrameListener(OgreMainWindow* baseApp, Ogre::RenderWindow* window,
                                                               Ogre::Camera* camera)
    : d_baseApp(baseApp) {
    // OIS setup
    OIS::ParamList paramList;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    // get window handle
    window->getCustomAttribute("WINDOW", &windowHnd);

    // fill param list
    windowHndStr << (unsigned int)windowHnd;
    paramList.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

#ifndef NDEBUG
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    paramList.insert(std::make_pair("x11_keyboard_grab", "false"));
    paramList.insert(std::make_pair("x11_mouse_grab", "false"));
    paramList.insert(std::make_pair("x11_mouse_hide", "false"));
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    paramList.insert(std::make_pair("w32_mouse", "DISCL_FOREGROUND"));
    paramList.insert(std::make_pair("w32_mouse", "DISCL_NONEXCLUSIVE"));
#endif
#endif

    // create input system
    d_inputManager = OIS::InputManager::createInputSystem(paramList);

    // create buffered keyboard

    if (d_inputManager->getNumberOfDevices(OIS::OISKeyboard) > 0) {
        d_keyboard = static_cast<OIS::Keyboard*>(d_inputManager->createInputObject(OIS::OISKeyboard, true));
        d_keyboard->setEventCallback(this);
    }

    // create buffered mouse
    if (d_inputManager->getNumberOfDevices(OIS::OISMouse) > 0) {
        d_mouse = static_cast<OIS::Mouse*>(d_inputManager->createInputObject(OIS::OISMouse, true));
        d_mouse->setEventCallback(this);

        unsigned int width, height, depth;
        int left, top;

        window->getMetrics(width, height, depth, left, top);
        const OIS::MouseState& mouseState = d_mouse->getMouseState();
        mouseState.width = width;
        mouseState.height = height;
    }

    // store inputs we want to make use of
    d_camera = camera;
    d_window = window;
}

InverseKinematicsFrameListener::~InverseKinematicsFrameListener() {
    if (d_inputManager) {
        d_inputManager->destroyInputObject(d_mouse);
        d_inputManager->destroyInputObject(d_keyboard);
        OIS::InputManager::destroyInputSystem(d_inputManager);
    }
}

//----------------------------------------------------------------------------//
bool InverseKinematicsFrameListener::frameStarted(const Ogre::FrameEvent& evt) {
    if (d_window->isClosed())
        return false;

    // update input system
    if (d_mouse)
        d_mouse->capture();
    if (d_keyboard)
        d_keyboard->capture();

    return true;
}

//----------------------------------------------------------------------------//
bool InverseKinematicsFrameListener::frameEnded(const Ogre::FrameEvent&) { return true; }

//----------------------------------------------------------------------------//
bool InverseKinematicsFrameListener::mouseMoved(const OIS::MouseEvent& e) {
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(e.state.X.abs, e.state.Y.abs);
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseWheelChange(e.state.Z.rel / 120.0f);

    return true;
}

//----------------------------------------------------------------------------//
bool InverseKinematicsFrameListener::keyPressed(const OIS::KeyEvent& e) {
    CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
    CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);

    return true;
}

//----------------------------------------------------------------------------//
bool InverseKinematicsFrameListener::keyReleased(const OIS::KeyEvent& e) {
    CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(e.key));

    return true;
}

//----------------------------------------------------------------------------//
bool InverseKinematicsFrameListener::mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID id) {
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertOISButtonToCegui(id));
    return true;
}

//----------------------------------------------------------------------------//
bool InverseKinematicsFrameListener::mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID id) {
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertOISButtonToCegui(id));
    return true;
}

//----------------------------------------------------------------------------//
CEGUI::MouseButton InverseKinematicsFrameListener::convertOISButtonToCegui(int buttonID) {
    using namespace OIS;

    switch (buttonID) {
    case OIS::MB_Left:
        return CEGUI::LeftButton;
    case OIS::MB_Right:
        return CEGUI::RightButton;
    case OIS::MB_Middle:
        return CEGUI::MiddleButton;
    default:
        return CEGUI::LeftButton;
    }
}

//----------------------------------------------------------------------------//
OIS::Mouse* InverseKinematicsFrameListener::getOISMouse() { return d_mouse; }

////////////////
///// WndEvtListener impl
////////////////

WndEvtListener::WndEvtListener(OIS::Mouse* mouse) : d_mouse(mouse) {}

//----------------------------------------------------------------------------//
void WndEvtListener::windowResized(Ogre::RenderWindow* rw) {
    CEGUI::System* const sys = CEGUI::System::getSingletonPtr();
    if (sys)
        sys->notifyDisplaySizeChanged(
            CEGUI::Sizef(static_cast<float>(rw->getWidth()), static_cast<float>(rw->getHeight())));

    const OIS::MouseState& mouseState = d_mouse->getMouseState();
    mouseState.width = rw->getWidth();
    mouseState.height = rw->getHeight();
}