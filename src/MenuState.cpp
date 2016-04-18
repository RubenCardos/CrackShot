#include "MenuState.h"
#include "PlayState.h"
#include "GameManager.h"
#include "string"
using namespace std;

template<> MenuState* Ogre::Singleton<MenuState>::msSingleton = 0;
std::vector<string> _recorridos;

void
MenuState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.----------------
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->createCamera("MenuCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.4, 0.4, 0.4));
  
   
  //Camara--------------------
  
  _camera->setNearClipDistance(5);
  _camera->setFarClipDistance(10000);
  //-----------------------------

  // Creacion de los elementos iniciales del mundo
  CreateInitialWorld();
  createGUI();

  _exitGame = false;

  
    
}

void
MenuState::exit ()
{
  //Salgo del estado------------------------------
  _sceneMgr->clearScene();
  _sceneMgr->destroyCamera("MenuCamera");
  _root->getAutoCreatedWindow()->removeAllViewports();
  //--------------------------------------------

  //Elimino interfaz------------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

  sheet->destroyChild("backgroundMenu");
  //-------------------------------------
}

void
MenuState::pause()
{
}

void
MenuState::resume()
{
  
}

void 
MenuState::CreateInitialWorld() {

}

bool
MenuState::frameStarted
(const Ogre::FrameEvent& evt)
{ 
  _deltaT = evt.timeSinceLastFrame;
  
  return true;
}


bool
MenuState::frameEnded
(const Ogre::FrameEvent& evt)
{

  _deltaT = evt.timeSinceLastFrame;
  

  //Salir del juego--
  if (_exitGame)
    return false;
  //-----------------
  return true;
}

void
MenuState::keyPressed
(const OIS::KeyEvent &e)
{
  // Tecla p --> PauseState.-------
  if (e.key == OIS::KC_P) {
    changeState(PlayState::getSingletonPtr());
  }
  //----------------- 

  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);
  //-------------------------------
}

void
MenuState::keyReleased
(const OIS::KeyEvent &e)
{
  //Salgo del juego---------------
  if (e.key == OIS::KC_ESCAPE) {
    _exitGame = true;
  }
  //-------------------------------
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(e.key));
  //-------------------------------
}

void
MenuState::mouseMoved
(const OIS::MouseEvent &e)
{

  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);
  //-------------------------------
}

void
MenuState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
  //-------------------------------
  
}

void
MenuState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
  //-------------------------------

}

MenuState*
MenuState::getSingletonPtr ()
{
return msSingleton;
}

MenuState&
MenuState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

CEGUI::MouseButton MenuState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
{
  //CEGUI--------------------------
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
  //------------------------------
}

bool
MenuState::quit(const CEGUI::EventArgs &e)
{
  _exitGame=true;
  return true;
}


void
MenuState::createGUI()
{
  //Limpiar interfaz del estado anterior-------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

  //-------------------------------------------------------
  CEGUI::Window* sheetBG =  CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","backgroundMenu");
  sheetBG->setPosition(CEGUI::UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetBG->setSize( CEGUI::USize(cegui_reldim(1),cegui_reldim(1)));
  sheetBG->setProperty("Image","BackgroundImageMenu");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");


  CEGUI::ListboxTextItem* itm;


  CEGUI::Listbox* editBox = static_cast<CEGUI::Listbox*> (CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Listbox","listbox"));
  editBox->setSize(CEGUI::USize(CEGUI::UDim(0.6,0),CEGUI::UDim(0.6,0)));
  editBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.20, 0),CEGUI::UDim(0.10, 0)));

  const CEGUI::Image* sel_img = &CEGUI::ImageManager::getSingleton().get("TaharezLook/MultiListSelectionBrush");
  
  std::vector<string> files = listDir("./data/Levels/*"); // ./Para directorio actual ./Carpeta/* para otra carpeta
  for(unsigned int i=0;i<files.size();i++){
    string aux=files[i];
    if(Ogre::StringUtil::endsWith(aux,".txt")){
      cout << "================ File: " << aux <<"================"<< endl;
      _recorridos.push_back(aux);
      string file=Ogre::StringUtil::split(aux,"/")[3];
      cout<<"File: " << file << endl;
      file=Ogre::StringUtil::replaceAll(file,".txt","");
      cout<<"File: " << file << endl;
      itm = new CEGUI::ListboxTextItem(file,0);
      itm->setFont("DickVanDyke-28");
      itm->setTextColours(CEGUI::Colour(0.0,0.8,0.5));
      itm->setSelectionBrushImage(sel_img);
      editBox->addItem(itm);
    }
    
  }
  //---------------------------------------------------

  CEGUI::Window* playButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","playButton");
  playButton->setText("[font='DickVanDyke'] Start");
  playButton->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.07,0)));
  playButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0),CEGUI::UDim(0.8,0)));
  playButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&MenuState::playB,this));

  sheetBG->addChild(playButton);
  sheetBG->addChild(editBox);
  sheet->addChild(sheetBG);

}


void
MenuState::updateGUI()
{

  
}

std::vector<string> 
MenuState::listDir(const string& pattern){
    glob_t glob_result;
    glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
    std::vector<string> files;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return files;
}

bool
MenuState::playB(const CEGUI::EventArgs &e)
{
  //Recupero el nivel selecionado-----------------------------------------
  CEGUI::Listbox* slistbox = static_cast<CEGUI::Listbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window->getRootWindow()->getChild("backgroundMenu")->getChild("listbox"));
  string listboxText=slistbox->getFirstSelectedItem()->getText().c_str();
  listboxText+=".txt";
  cout << listboxText << endl;
  for(unsigned int i=0;i<_recorridos.size();i++){
    string aux=_recorridos[i];
    if(Ogre::StringUtil::endsWith(aux,listboxText)){
      GameManager::getSingletonPtr()->setLevel(aux);
      changeState(PlayState::getSingletonPtr());
    }
  }
  return true;
}