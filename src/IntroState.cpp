#include "IntroState.h"
#include "PlayState.h"
#include "MenuState.h"

using namespace CEGUI;

template<> IntroState* Ogre::Singleton<IntroState>::msSingleton = 0;

void
IntroState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  _sceneMgr = _root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
  _camera = _sceneMgr->createCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.4, 0.4, 0.4));

  GameManager::getSingletonPtr()->_mainTrack = GameManager::getSingletonPtr()->_pTrackManager->load("BGMusic.mp3");
  GameManager::getSingletonPtr()->_mainTrack->play();


  _exitGame = false;

  //SN Pivote-------------------
  SceneNode* _pivot = _sceneMgr->createSceneNode("PivotSN");
  _pivot->setPosition(Ogre::Vector3(0,0,0));
  _pivot->attachObject(_camera);
  _sceneMgr->getRootSceneNode()->addChild(_pivot);
  //----------------------------

  //Camara--------------------
  _camera->setPosition(Ogre::Vector3(400,400,400));//el tercer parametro hace que se aleje mas la camara, el segundo para que rote hacia arriba o hacia abajo
  _camera->lookAt(Ogre::Vector3(0,1,0));//bajar el 60 un poco
  _camera->setNearClipDistance(5);
  _camera->setFarClipDistance(10000);
  //-----------------------------

  //Escena Intro-------------------------------------------------
  SceneNode* _nodeFair1 = _sceneMgr->createSceneNode("Fair1SN");
  Entity* _entFair1 = _sceneMgr->createEntity("fair1.mesh");
  _nodeFair1->attachObject(_entFair1);
  _sceneMgr->getRootSceneNode()->addChild(_nodeFair1);

  SceneNode* _nodeFair2 = _sceneMgr->createSceneNode("Fair2SN");
  Entity* _entFair2 = _sceneMgr->createEntity("fair2.mesh");
  _nodeFair2->attachObject(_entFair2);
  _sceneMgr->getRootSceneNode()->addChild(_nodeFair2);

  SceneNode* _nodeWall = _sceneMgr->createSceneNode("wall");
  Entity* _entWall = _sceneMgr->createEntity("wall.mesh");
  _nodeWall->attachObject(_entWall);
  _sceneMgr->getRootSceneNode()->addChild(_nodeWall);
  //-----------------------------------------------------------------


  //LUCES------------------------------------------------
  Light* _sunLight = _sceneMgr->createLight("SunLight");
  _sunLight->setPosition(200, 200, 200);
  _sunLight->setType(Light::LT_SPOTLIGHT);
  _sunLight->setDiffuseColour(.20, .20, 0);
  _sunLight->setSpotlightRange(Degree(30), Degree(50));

  Ogre::Vector3 _dir= -_sunLight->getPosition().normalisedCopy();
  _sunLight->setDirection(_dir);


  Light* _directionalLight = _sceneMgr->createLight("DirectionalLight");
  _directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
  //directionalLight->setDiffuseColour(.60, .60, 0);
  _directionalLight->setDirection(Ogre::Vector3(0, -1, 1));

  //-----------------------------------------------------

  //SkyBox-------------------------------------
  _sceneMgr->setSkyBox(true, "MaterialSkybox");
  //-------------------------------------------

  //Creamos interfaces en CEGUI
  createGUI();
  _fullscreen=false;
  _resWidth=1200;
  _resHeigt=800;
  //---------------------------
}
void IntroState::createGUI()
{
  //CEGUI
  renderer = &CEGUI::OgreRenderer::bootstrapSystem();
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
  CEGUI::SchemeManager::getSingleton().createFromFile("OgreTray.scheme");
  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
  CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-12");

  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseArrow");

  // load all the fonts (if they are not loaded yet)
  CEGUI::FontManager::getSingleton().createAll("*.font", "Fonts");
    
  //Sheet
  CEGUI::Window* sheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Sheet");

  //Para liberar el raton----------------------------------------------------------------------------------------
  CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(-mousePos.d_x,-mousePos.d_y);
  //-------------------------------------------------------------------------------------------------------------

  //Cargo la imagen
  ImageManager::getSingleton().addFromImageFile("BackgroundImage","logo.png");
  ImageManager::getSingleton().addFromImageFile("BackgroundImageRecords","records.png");
  ImageManager::getSingleton().addFromImageFile("BackgroundImageCredits","credits.png");
  ImageManager::getSingleton().addFromImageFile("BackgroundImageMenu","wallpaper.png");
  ImageManager::getSingleton().addFromImageFile("BackgroundImageBar","barra.png");
  ImageManager::getSingleton().addFromImageFile("BackgroundImageMeter","medidor.png");
  ImageManager::getSingleton().addFromImageFile("BackgroundImageFinish","finish.png");
  CEGUI::ImageManager::getSingleton().addFromImageFile("BackgroundImageArrow","flecha.png");
  CEGUI::ImageManager::getSingleton().addFromImageFile("BackgroundImagePower","RainOfArrows.png");
  ImageManager::getSingleton().addFromImageFile("BackgroundImageOptions","options.png");
  ImageManager::getSingleton().addFromImageFile("BackgroundImageControls","controls.png");

  //Sheet
  Window* sheetBG =  WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_wnd");
  sheetBG->setPosition(CEGUI::UVector2(CEGUI::UDim(0.14, 0),CEGUI::UDim(0.06, 0)));
  sheetBG->setSize( CEGUI::USize(CEGUI::UDim(0.70, 0), CEGUI::UDim(0.45, 0)));
  sheetBG->setProperty("Image","BackgroundImage");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");
   
   //Buttons
  CEGUI::Window* playButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","playButton");
  playButton->setText("[font='DickVanDyke'] Play");
  playButton->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.06,0)));
  playButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.40,0),CEGUI::UDim(0.50,0)));
  playButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::play,this));

  CEGUI::Window* infoButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","infoButton");
  infoButton->setText("[font='DickVanDyke'] How To Play");
  infoButton->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.06,0)));
  infoButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.40,0),CEGUI::UDim(0.58,0)));
  infoButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::info,this));

  CEGUI::Window* optionsButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","optionsButton");
  optionsButton->setText("[font='DickVanDyke'] Options");
  optionsButton->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.06,0)));
  optionsButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.40,0),CEGUI::UDim(0.66,0)));
  optionsButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::options,this));

  CEGUI::Window* recordsButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","recordsButton");
  recordsButton->setText("[font='DickVanDyke'] Records");
  recordsButton->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.06,0)));
  recordsButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.40,0),CEGUI::UDim(0.74,0)));
  recordsButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::records,this));

  CEGUI::Window* creditsButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","creditsButton");
  creditsButton->setText("[font='DickVanDyke'] Credits");
  creditsButton->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.06,0)));
  creditsButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.40,0),CEGUI::UDim(0.82,0)));
  creditsButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::credits,this));

  CEGUI::Window* quitButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","quitButton");
  quitButton->setText("[font='DickVanDyke'] Exit");
  quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.06,0)));
  quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.40,0),CEGUI::UDim(0.90,0)));
  quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::quit,this));


  //Attaching buttons
  sheet->addChild(playButton);
  sheet->addChild(infoButton);
  sheet->addChild(optionsButton);
  sheet->addChild(recordsButton);
  sheet->addChild(creditsButton);
  sheet->addChild(quitButton);
  sheet->addChild(sheetBG);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);

  
}

bool
IntroState::back(const CEGUI::EventArgs &e)
{
  
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  
  if(_credits==true){
    sheet->destroyChild("background_credits");
  }else{
    if(_options==true){
      sheet->destroyChild("background_options");
    }else{
      if(_info){
        sheet->destroyChild("background_info");
      }else{
        sheet->destroyChild("background_records");
      }
       
    }
    
  }


  return true;
}


bool
IntroState::credits(const CEGUI::EventArgs &e)
{

  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  _credits=true;
  _options=false;
  _info=false;

  //Sheet Creditos
  Window* sheetBG =  WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_credits");
  sheetBG->setPosition( UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetBG->setSize( USize(cegui_reldim(1),cegui_reldim(1)));
  sheetBG->setProperty("Image","BackgroundImageCredits");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* backButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","back");
  backButton->setText("[font='DickVanDyke'] Back ");
  backButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  backButton->setXPosition(UDim(0.66f, 0.0f));
  backButton->setYPosition(UDim(0.85f, 0.0f));
  backButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::back,this));
  
  sheetBG->addChild(backButton);
  sheet->addChild(sheetBG);
  return true;
}

bool
IntroState::records(const CEGUI::EventArgs &e)
{
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  _credits=false;
  _options=false;
  _info=false;

  //Sheet Records
  Window* sheetBG =  WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_records");
  sheetBG->setPosition( UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetBG->setSize( USize(cegui_reldim(1),cegui_reldim(1)));
  sheetBG->setProperty("Image","BackgroundImageRecords");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* backButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","back");
  backButton->setText("[font='DickVanDyke'] Back ");
  backButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  backButton->setXPosition(UDim(0.66f, 0.0f));
  backButton->setYPosition(UDim(0.85f, 0.0f));
  backButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::back,this));
  
  CEGUI::Window* text = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","text");
  text->setText("[font='DickVanDyke']"+readRecords());
  text->setSize(CEGUI::USize(CEGUI::UDim(0.50,0),CEGUI::UDim(0.70,0)));
  text->setXPosition(UDim(0.1f, 0.0f));
  text->setYPosition(UDim(0.15f, 0.0f));
  text->setProperty("FrameEnabled","False");
  text->setProperty("BackgroundEnabled", "False");
  text->setProperty("HorzFormatting", "RightAligned");


  sheetBG->addChild(backButton);
  sheetBG->addChild(text);
  sheet->addChild(sheetBG);
  
  return true;
}
void
IntroState::exit()
{
  _sceneMgr->clearScene();
  _sceneMgr->destroyCamera("IntroCamera");
  _root->getAutoCreatedWindow()->removeAllViewports();


  //Destruir interfaz--------------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

  sheet->destroyChild("background_wnd");
  sheet->destroyChild("playButton");
  sheet->destroyChild("infoButton");
  sheet->destroyChild("optionsButton");
  sheet->destroyChild("recordsButton");
  sheet->destroyChild("creditsButton");
  sheet->destroyChild("quitButton");
  //--------------------------------------------

  GameManager::getSingletonPtr()->_mainTrack->unload();
}

void
IntroState::pause ()
{
}

void
IntroState::resume ()
{
}

bool
IntroState::frameStarted
(const Ogre::FrameEvent& evt) 
{
 
  //Rotacion con el pivote----------
  SceneNode* _pivot = _sceneMgr->getSceneNode("PivotSN");
  _pivot->yaw(Ogre::Degree(0.1));
  //--------------------------------
  
  return true;
}

bool
IntroState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
IntroState::keyPressed
(const OIS::KeyEvent &e)
{
  // Transición al siguiente estado.
  // Espacio --> PlayState
  if (e.key == OIS::KC_SPACE) {
    changeState(MenuState::getSingletonPtr());
  }

  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);

}

void
IntroState::keyReleased
(const OIS::KeyEvent &e )
{
  if (e.key == OIS::KC_ESCAPE) {
    _exitGame = true;
  }

  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(e.key));
}

void
IntroState::mouseMoved
(const OIS::MouseEvent &e)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);  
  
}

void
IntroState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
}

void
IntroState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
}

IntroState*
IntroState::getSingletonPtr ()
{
return msSingleton;
}

IntroState&
IntroState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}



CEGUI::MouseButton IntroState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
{
  CEGUI::MouseButton ceguiId;
  switch(id)
    {
    case OIS::MB_Left:
      ceguiId = CEGUI::LeftButton;
      break;
    case OIS::MB_Right:
      ceguiId = CEGUI::RightButton;
      break;
    case OIS::MB_Middle:
      ceguiId = CEGUI::MiddleButton;
      break;
    default:
      ceguiId = CEGUI::LeftButton;
    }
  return ceguiId;
}

bool
IntroState::play(const CEGUI::EventArgs &e)
{
  changeState(MenuState::getSingletonPtr());
  return true;
}

bool
IntroState::quit(const CEGUI::EventArgs &e)
{
  _exitGame=true;
  return true;
}

void 
IntroState::createRecordsFile(){
  cout << "Fichero inexistente o faltan permisos para abrirlo, creando archivo e records..." << endl;
  ofstream archivo;  // objeto de la clase ofstream
  archivo.open("Records.txt");
  for(int i=0;i<9;i++){
    archivo  << "AAA 000" << endl;
  }
  archivo.close();
}

std::string 
IntroState::readRecords(){
    
    std::string res="";

    fstream fichero;//Fichero
    std::string frase;//Auxiliar

    fichero.open ( "Records.txt" , ios::in);
    if (fichero.is_open()) {
      while (getline (fichero,frase)) {
        std::string nombre=Ogre::StringUtil::split (frase," ")[0];
        int punt = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase," ")[1]);
        cout << "\tNombre: "<< nombre << " Puntuacion " << punt << endl;
        res+= nombre+"     "+Ogre::StringUtil::split (frase," ")[1]+"\n";
      }
      fichero.close();    
    }else{
        createRecordsFile();
        fichero.open ( "Records.txt" , ios::in);
        if (fichero.is_open()) {
          while (getline (fichero,frase)) {
            std::string nombre=Ogre::StringUtil::split (frase," ")[0];
            int punt = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase," ")[1]);
            cout << "\tNombre: "<< nombre << " Puntuacion " << punt << endl;
            res+= nombre+"     "+Ogre::StringUtil::split (frase," ")[1]+"\n";
          }
          fichero.close();   
        }
    }
  return res;
}

bool
IntroState::info(const CEGUI::EventArgs &e)
{
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  _credits=false;
  _options=false;
  _info=true;

  //Sheet Creditos
  Window* sheetBG =  WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_info");
  sheetBG->setPosition( UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetBG->setSize( USize(cegui_reldim(1),cegui_reldim(1)));
  sheetBG->setProperty("Image","BackgroundImageControls");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* backButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","back");
  backButton->setText("[font='DickVanDyke'] Back ");
  backButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  backButton->setXPosition(UDim(0.66f, 0.0f));
  backButton->setYPosition(UDim(0.85f, 0.0f));
  backButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::back,this));
  
  sheetBG->addChild(backButton);
  sheet->addChild(sheetBG);
  return true;
}

bool
IntroState::options(const CEGUI::EventArgs &e)
{
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  _credits=false;
  _options=true;
  _info=false;

  //Sheet Records
  Window* sheetBG =  WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_options");
  sheetBG->setPosition( UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetBG->setSize( USize(cegui_reldim(1),cegui_reldim(1)));
  sheetBG->setProperty("Image","BackgroundImageOptions");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* backButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","back");
  backButton->setText("[font='DickVanDyke'] Back ");
  backButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  backButton->setXPosition(UDim(0.66f, 0.0f));
  backButton->setYPosition(UDim(0.85f, 0.0f));
  backButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::back,this));
  
  CEGUI::Window* applyButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","apply");
  applyButton->setText("[font='DickVanDyke'] apply ");
  applyButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  applyButton->setXPosition(UDim(0.66f, 0.0f));
  applyButton->setYPosition(UDim(0.77f, 0.0f));
  applyButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::apply,this));

  CEGUI::Window* text = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","text");
  text->setText("[font='DickVanDyke'] Mouse Speed");
  text->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.1,0)));
  text->setPosition(CEGUI::UVector2(CEGUI::UDim(0.12,0),CEGUI::UDim(0.32,0)));
  text->setProperty("FrameEnabled","False");
  text->setProperty("BackgroundEnabled", "False");
  text->setProperty("HorzFormatting", "RightAligned");

  CEGUI::Slider* sb = static_cast<CEGUI::Slider*> (CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Slider","SliderSpeed"));
  sb->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15,0),CEGUI::UDim(0.4,0)));
  sb->setMaxValue(40.0f);
  sb->setClickStep(1.0f);
  sb->setCurrentValue(20.0f); 
  sb->subscribeEvent(Slider::EventValueChanged, Event::Subscriber(&IntroState::onSliderValueChanged, this));

  CEGUI::Window* sbText = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textSliderSpeed");
  sbText->setText("[font='DickVanDyke']20");
  sbText->setSize(CEGUI::USize(CEGUI::UDim(0.05,0),CEGUI::UDim(0.08,0)));
  sbText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.36,0),CEGUI::UDim(0.38,0)));
  sbText->setProperty("FrameEnabled","False");
  sbText->setProperty("BackgroundEnabled", "False");
  sbText->setProperty("HorzFormatting", "RightAligned");

  CEGUI::Window* text2 = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","text2");
  text2->setText("[font='DickVanDyke'] Auto-Reload Arrows");
  text2->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.06,0)));
  text2->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15,0),CEGUI::UDim(0.46,0)));
  text2->setProperty("FrameEnabled","False");
  text2->setProperty("BackgroundEnabled", "False");
  text2->setProperty("HorzFormatting", "LeftAligned");

  CEGUI::ToggleButton* cb = static_cast<CEGUI::ToggleButton*> (CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Checkbox","CBAutoReload"));
  cb->setPosition(CEGUI::UVector2(CEGUI::UDim(0.08,0),CEGUI::UDim(0.52,0)));

  CEGUI::Window* resetButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","resetButton");
  resetButton->setText("[font='DickVanDyke'] Reset Records ");
  resetButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  resetButton->setXPosition(UDim(0.40f, 0.0f));
  resetButton->setYPosition(UDim(0.85f, 0.0f));
  resetButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::resetRecords,this));

  CEGUI::Window* configButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","configButton");
  configButton->setText("[font='DickVanDyke'] Restore Config ");
  configButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  configButton->setXPosition(UDim(0.40f, 0.0f));
  configButton->setYPosition(UDim(0.77f, 0.0f));
  configButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::resetConfig,this));

  CEGUI::Window* text3 = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","FullscreenText");
  text3->setText("[font='DickVanDyke'] Fullscreen");
  text3->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.06,0)));
  text3->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15,0),CEGUI::UDim(0.57,0)));
  text3->setProperty("FrameEnabled","False");
  text3->setProperty("BackgroundEnabled", "False");
  text3->setProperty("HorzFormatting", "LeftAligned");

  CEGUI::ToggleButton* cbFullscreen = static_cast<CEGUI::ToggleButton*> (CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Checkbox","CBFullscreen"));
  cbFullscreen->setPosition(CEGUI::UVector2(CEGUI::UDim(0.08,0),CEGUI::UDim(0.62,0)));
  cbFullscreen->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged,CEGUI::Event::Subscriber(&IntroState::tbFullscreenChanged,this));

  CEGUI::Window* text4 = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","ResolutionText");
  text4->setText("[font='DickVanDyke'] Resolution");
  text4->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.06,0)));
  text4->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5,0),CEGUI::UDim(0.35,0)));
  text4->setProperty("FrameEnabled","False");
  text4->setProperty("BackgroundEnabled", "False");
  text4->setProperty("HorzFormatting", "LeftAligned");

  


  CEGUI::Listbox* lbRes = static_cast<CEGUI::Listbox*> (CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Listbox","lbRes"));
  lbRes->setSize(CEGUI::USize(CEGUI::UDim(0.3,0),CEGUI::UDim(0.3,0)));
  lbRes->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5, 0),CEGUI::UDim(0.40, 0)));
  lbRes->subscribeEvent(Listbox::EventSelectionChanged,CEGUI::Event::Subscriber(&IntroState::changeResolution,this));
  
  const CEGUI::Image* sel_img = &CEGUI::ImageManager::getSingleton().get("TaharezLook/MultiListSelectionBrush");
  
  CEGUI::ListboxTextItem* itm;
  itm = new CEGUI::ListboxTextItem("800x600",0);
  itm->setFont("DickVanDyke-28");
  itm->setTextColours(CEGUI::Colour(0.0,0.8,0.5));
  itm->setSelectionBrushImage(sel_img);
  lbRes->addItem(itm);
  
  CEGUI::ListboxTextItem* itm2;
  itm2 = new CEGUI::ListboxTextItem("1200x800",1);
  itm2->setFont("DickVanDyke-28");
  itm2->setTextColours(CEGUI::Colour(0.0,0.8,0.5));
  itm2->setSelectionBrushImage(sel_img);
  lbRes->addItem(itm2);

  CEGUI::ListboxTextItem* itm3;
  itm3 = new CEGUI::ListboxTextItem("1920x1020",2);
  itm3->setFont("DickVanDyke-28");
  itm3->setTextColours(CEGUI::Colour(0.0,0.8,0.5));
  itm3->setSelectionBrushImage(sel_img);
  lbRes->addItem(itm3);
    
  

  sheetBG->addChild(text4);
  sheetBG->addChild(lbRes);
  sheetBG->addChild(cb);
  sheetBG->addChild(text2);
  sheetBG->addChild(cbFullscreen);
  sheetBG->addChild(text3);
  sheetBG->addChild(sbText);
  sheetBG->addChild(sb);
  sheetBG->addChild(applyButton);
  sheetBG->addChild(backButton);
  sheetBG->addChild(resetButton);
  sheetBG->addChild(configButton);
  sheetBG->addChild(text);
  sheet->addChild(sheetBG);
    

  if(_fullscreen){
    cbFullscreen->setSelected(true);
  }else{
    cbFullscreen->setSelected(false);
  }

  return true;
}

bool 
IntroState::onSliderValueChanged(const CEGUI::EventArgs &e){

  CEGUI::Slider* slider = static_cast<CEGUI::Slider*>(static_cast<const CEGUI::WindowEventArgs&>(e).window->getRootWindow()->getChild("background_options")->getChild("SliderSpeed"));
  float aux=slider->getCurrentValue();
  CEGUI::Window* sbText =static_cast<CEGUI::Slider*>(static_cast<const CEGUI::WindowEventArgs&>(e).window->getRootWindow()->getChild("background_options")->getChild("textSliderSpeed"));
  if(aux<5.0){
    aux=5.0;
  }
  sbText->setText("[font='DickVanDyke']"+Ogre::StringConverter::toString( (int)aux ));
  return true;
}

bool 
IntroState::apply(const CEGUI::EventArgs &e){
  
  CEGUI::Slider* slider = static_cast<CEGUI::Slider*>(static_cast<const CEGUI::WindowEventArgs&>(e).window->getRootWindow()->getChild("background_options")->getChild("SliderSpeed"));
  float aux=slider->getCurrentValue();
  
  CEGUI::ToggleButton* cb = static_cast<CEGUI::ToggleButton*>(static_cast<const CEGUI::WindowEventArgs&>(e).window->getRootWindow()->getChild("background_options")->getChild("CBAutoReload"));

  ofstream archivo;  // objeto de la clase ofstream
  archivo.open("data/Config.txt");
  archivo<<"MouseSpeed = "+Ogre::StringConverter::toString((int)aux)<< endl;
  archivo<<"AutoReaload = "+Ogre::StringConverter::toString(cb->isSelected())<< endl;
  archivo.close();

  //Cambio de resolucion-----------
  if(_fullscreen){
    _root->getAutoCreatedWindow()->resize(1920,1200);
    renderer->setDisplaySize(CEGUI::Sizef(1920,1200));
    _root->getAutoCreatedWindow()->setFullscreen(true,1920,1200);
  }else{
    _root->getAutoCreatedWindow()->resize(_resWidth,_resHeigt);
    renderer->setDisplaySize(CEGUI::Sizef(_resWidth,_resHeigt));
    _root->getAutoCreatedWindow()->setFullscreen(false,_resWidth,_resHeigt);
  }
  
  //-------------------------------
  return true;
}

bool
IntroState::resetRecords(const CEGUI::EventArgs &e){
  cout << "Reset Records" << endl;
  IntroState::createRecordsFile();
  return true;
}

bool 
IntroState::resetConfig(const CEGUI::EventArgs &e){
  //Fichero de configuracion por defecto---
  cout << "Cargando configuracion por defecto" << endl;
  ofstream archivo;  // objeto de la clase ofstream
  archivo.open("data/Config.txt");
  archivo<<"MouseSpeed = 20"<< endl;
  archivo<<"AutoReaload = false"<< endl;
  archivo.close();
  //----------------------------------------------------------------
  return true;
}

bool 
IntroState::tbFullscreenChanged(const CEGUI::EventArgs &e){
  CEGUI::ToggleButton* cb = static_cast<CEGUI::ToggleButton*>(static_cast<const CEGUI::WindowEventArgs&>(e).window->getRootWindow()->getChild("background_options")->getChild("CBFullscreen"));
  if(cb->isSelected()){
    //_root->getAutoCreatedWindow()->resize(_resWidth,_resHeigt);
    //renderer->setDisplaySize(CEGUI::Sizef(_resWidth,_resHeigt));//CESAR
    //_root->getAutoCreatedWindow()->setFullscreen(true,_resWidth,_resHeigt);//CESAR
    _fullscreen=true;
  }else{
    //_root->getAutoCreatedWindow()->resize(_resWidth,_resHeigt);
    //renderer->setDisplaySize(CEGUI::Sizef(_resWidth,_resHeigt));//CESAR
    //_root->getAutoCreatedWindow()->setFullscreen(false,_resWidth,_resHeigt);//CESAR
    _fullscreen=true;
  }

  return true;
}

bool 
IntroState::changeResolution(const CEGUI::EventArgs &e){
  CEGUI::Listbox* lb = static_cast<CEGUI::Listbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window->getRootWindow()->getChild("background_options")->getChild("lbRes"));
  string sel=lb->getFirstSelectedItem()->getText().c_str();
  _resWidth=Ogre::StringConverter::parseInt(Ogre::StringUtil::split(sel,"x")[0]);
  _resHeigt=Ogre::StringConverter::parseInt(Ogre::StringUtil::split(sel,"x")[1]);
  cout << _resWidth << endl;
  cout << _resHeigt << endl;
  return true;
}