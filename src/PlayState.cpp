#include "PlayState.h"
#include "PauseState.h"
#include "GameOverState.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"
#include "Shapes/OgreBulletCollisionsSphereShape.h"
#include "OgreBulletCollisionsRay.h"

using namespace std;

template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

//Recorrido----------------------
Vector3 _now;
Vector3 _next;
std::vector<Vector3> _points;
std::vector<SceneNode*> _targets;
//-------------------------------

//Camara-------------------------
int _speedCam;
Vector3 _vnCam;
int _desp;
//-------------------------------

//Arco y flecha graficos---
SceneNode* _arrowI;
SceneNode* _bow;
//-------------------------

//Fuerza de las flechas----
float _force;
float _forceInc;
float _maxForce;
bool _mousePressed;
bool _autoReaload;
//-------------------------

//Puntuacion---
int _punt;
//-------------

//GUI-----------
int _nFramesGUIImpact;
bool _impact;
//--------------

//Lvl-----------
string _lvl;
int _arrows;
bool _inGame;
//--------------

//SuperPower----
bool _power;
int _superArrows;
//--------------

//Globo---------
bool _upGlobe;
int _globeUpDesp;
//--------------

void
PlayState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();


  GameManager::getSingletonPtr()->_mainTrack = GameManager::getSingletonPtr()->_pTrackManager->load("BGGame.ogg");
  GameManager::getSingletonPtr()->_mainTrack->play();


  // Se recupera el gestor de escena y la cámara.----------------
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->createCamera("PlayCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.4, 0.4, 0.4));
  
   
  //Camara--------------------
  _camera->setPosition(Ogre::Vector3(415,34,45));//el tercer parametro hace que se aleje mas la camara, el segundo para que rote hacia arriba o hacia abajo
  _camera->lookAt(Ogre::Vector3(0,0,0));//bajar el 60 un poco
  _camera->setNearClipDistance(5);
  _camera->setFarClipDistance(10000);
  //-----------------------------
  
  _numEntities = 0;    // Numero de Shapes instanciadas
  _timeLastObject = 0; // Tiempo desde que se añadio el ultimo objeto

  // Creacion del modulo de debug visual de Bullet ------------------
  _debugDrawer = new OgreBulletCollisions::DebugDrawer();
  _debugDrawer->setDrawWireframe(true);  
  SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode("debugNode", Vector3::ZERO);
  node->attachObject(static_cast <SimpleRenderable *>(_debugDrawer));
  //-----------------------------------------------------------------

  // Creacion del mundo (definicion de los limites y la gravedad)----
  AxisAlignedBox worldBounds = AxisAlignedBox (Vector3 (-10000, -10000, -10000), Vector3 (10000,  10000,  10000));
  Vector3 gravity = Vector3(0, -9.8, 0);

  _world = new OgreBulletDynamics::DynamicsWorld(_sceneMgr,worldBounds, gravity);
  _world->setDebugDrawer (_debugDrawer);
  _world->setShowDebugShapes(false);  // Muestra los collision shapes
  //-----------------------------------------------------------------

  //Creacion de los elementos iniciales del mundo---
  CreateInitialWorld();
  //------------------------------------------------
  
  //Creo la interfaz---
  createGUI();
  //-------------------

  //Posicion de la camara, _now y _next
  _now=_camera->getPosition();
  _next=Vector3(-1,-1,-1);
  //-----------------------------------

  // Lista de puntos por lo que pasara la camara ----
  
  //Recupero el nivel selecionado--------------------
  _lvl=GameManager::getSingletonPtr()->getLevel();
  cout << "LVL: " << _lvl << endl;
  //-------------------------------------------------

  //Leo el fichero-----------------------------------
  fstream fichero;//Fichero
  string frase;//Auxiliar
  fichero.open(_lvl.c_str(),ios::in);
  if (fichero.is_open()) {
    while (getline (fichero,frase)) {
      int x = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[0]);
      int y = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[1]);
      int z = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[2]);
      Vector3 aux = Vector3(x,y,z);
      _points.push_back(aux);
    }
    fichero.close();    
  }
  //-------------------------------------------------


  //inicializacion de variables--------------------------
  //Cargo la configuracion---
  loadConfig();
  //-------------------------

  _speedCam = 30;
  //_desp=20;
  _punt=0;

  _force=0.0;
  _forceInc=1.0;
  _maxForce=200.0;
  _mousePressed=false;
  //_autoReaload=false;

  //GUI-----------
  _nFramesGUIImpact = 120;
  _impact=false;
  //--------------

  //Lvl-----------
  _arrows = 6;
  _inGame=true;
  //--------------

  //SuperPower----
  _power=false;
  _superArrows=11;
  //--------------

  //Globo---------
  _upGlobe=true;
  _globeUpDesp=900;
  //--------------

  //------------------------------------------------------
  //--------------------------------------------------------
  


  _exitGame = false;
}

void
PlayState::exit ()
{
  //Salgo del estado------------------------------
  _sceneMgr->clearScene();
  _sceneMgr->destroyCamera("PlayCamera");
  _root->getAutoCreatedWindow()->removeAllViewports();
  //--------------------------------------------

  //Variables----------
  _points.clear();
  //------------------

  //Limpio la interfaz---------------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  
  sheet->destroyChild("quitButton2");
  sheet->destroyChild("pauseButton");
  sheet->destroyChild("textPoints");
  sheet->destroyChild("textR");
  sheet->destroyChild("textImpact");
  sheet->destroyChild("bar");
  sheet->destroyChild("meter");


  sheet->destroyChild("img1");
  sheet->destroyChild("img2");
  sheet->destroyChild("img3");
  sheet->destroyChild("img4");
  sheet->destroyChild("img5");
  sheet->destroyChild("img6");
  sheet->destroyChild("imgPower");
  
  //---------------------------------------------

  GameManager::getSingletonPtr()->_mainTrack->unload();
}

void
PlayState::pause()
{
}

void
PlayState::resume()
{
  
}

void 
PlayState::CreateInitialWorld() {
  
  //Suelo Infinito NO TOCAR---------------------------------
  Plane plane1(Vector3(0,1,0), 0);    // Normal y distancia
  MeshManager::getSingleton().createPlane("p1",
  ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1,
  800, 450, 1, 1, true, 1, 20, 20, Vector3::UNIT_Z);
  //--------------------------------------------------------

  //Suelo Grafico-----------------------------------------------
  SceneNode* _groundNode = _sceneMgr->createSceneNode("ground");
  Entity* _groundEnt = _sceneMgr->createEntity("planeEnt", "p1");
  _groundEnt->setMaterialName("Ground");
  _groundNode->attachObject(_groundEnt);
  _sceneMgr->getRootSceneNode()->addChild(_groundNode);
  //------------------------------------------------------------
 	
  // Creamos forma de colision para el plano ----------------------- 
  OgreBulletCollisions::CollisionShape *Shape;
  Shape = new OgreBulletCollisions::StaticPlaneCollisionShape
    (Ogre::Vector3(0,1,0), 0);   // Vector normal y distancia
  OgreBulletDynamics::RigidBody *rigidBodyPlane = new 
    OgreBulletDynamics::RigidBody("rigidBodyPlane", _world);

  // Creamos la forma estatica (forma, Restitucion, Friccion) ------
  rigidBodyPlane->setStaticShape(Shape, 0.1, 0.8); 

  // Anadimos los objetos Shape y RigidBody ------------------------
  _shapes.push_back(Shape);  
  _bodies.push_back(rigidBodyPlane);
  
  //Leo el fichero-----------------------------------
  fstream fichero;//Fichero
  string frase;//Auxiliar
  fichero.open("data/Targets.txt",ios::in);
  if (fichero.is_open()) {
    while (getline (fichero,frase)) {
      
      int x = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[0]);
      int y = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[1]);
      int z = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[2]);
      int rot = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[3]);
      
      Vector3 aux = Vector3(x,y,z);
      Quaternion q = Quaternion(Degree(rot), Vector3::UNIT_Y);
      
      //Añado diana----------------
      AddStaticObject(aux,q);
      //---------------------------

    }
    fichero.close();    
  }
  //-------------------------------------------------

  //ESCENARIO-------------------------------------------------
  
  //Nodo Grafico------------------------------------------------
  SceneNode* _nodeFair1 = _sceneMgr->createSceneNode("Fair1SN");
  Entity* _entFair1 = _sceneMgr->createEntity("fair1.mesh");
  _nodeFair1->attachObject(_entFair1);
  _sceneMgr->getRootSceneNode()->addChild(_nodeFair1);
  //------------------------------------------------------------

  //Nodo Grafico------------------------------------------------
  SceneNode* _nodeFair2 = _sceneMgr->createSceneNode("Fair2SN");
  Entity* _entFair2 = _sceneMgr->createEntity("fair2.mesh");
  _nodeFair2->attachObject(_entFair2);
  _sceneMgr->getRootSceneNode()->addChild(_nodeFair2);
  //------------------------------------------------------------

  //Globo------------------------------------------------
  SceneNode* _nodeGlobo = _sceneMgr->createSceneNode("GloboSN");
  Entity* _entGlobo = _sceneMgr->createEntity("globe.mesh");
  _nodeGlobo->attachObject(_entGlobo);
  _nodeGlobo->translate(Vector3(0,10,0));
  _sceneMgr->getRootSceneNode()->addChild(_nodeGlobo);
  //------------------------------------------------------------
  
  //SkyBox-------------------------------------
  _sceneMgr->setSkyBox(true, "MaterialSkybox");
  //-------------------------------------------
  
  //Flecha Inicial y Arco-----------------------------
  Entity *entAI = _sceneMgr->createEntity("entAI", "arrow.mesh");
  _arrowI = _sceneMgr->getRootSceneNode()->createChildSceneNode("ArrowISN");
  _arrowI->attachObject(entAI);

  Entity *entBow = _sceneMgr->createEntity("entBow", "bow.mesh");
  _bow = _sceneMgr->createSceneNode("BowSN");
  _bow->pitch(Degree(90));
  _bow->setPosition(1.5,-0.6,0.4);
  _bow->attachObject(entBow);
  _arrowI->addChild(_bow);
  //-------------------------------------------

  //Animacion tensar-----------------------------------------------------------
  /*_animBowNormal = _sceneMgr->getEntity("entBow")->getAnimationState("tensar"); //tensarNormal o tensarFast
  _animBowNormal->setEnabled(true);
  _animBowNormal->setLoop(false);
  _animBowNormal->setTimePosition(0.0);*/
  //---------------------------------------------------------------------------

  //LUCES------------------------------------------------
  Light* _sunLight = _sceneMgr->createLight("SunLight");
  _sunLight->setPosition(200, 200, 200);
  _sunLight->setType(Light::LT_SPOTLIGHT);
  _sunLight->setDiffuseColour(.20, .20, 0);
  _sunLight->setSpotlightRange(Degree(30), Degree(50));

  Vector3 _dir= -_sunLight->getPosition().normalisedCopy();
  _sunLight->setDirection(_dir);


  Light* _directionalLight = _sceneMgr->createLight("DirectionalLight");
  _directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
  //directionalLight->setDiffuseColour(.60, .60, 0);
  _directionalLight->setDirection(Ogre::Vector3(0, -1, 1));

  //-----------------------------------------------------

}

void 
PlayState::AddStaticObject(Vector3 pos,Quaternion rot) {
  
  Entity* entAim = _sceneMgr->createEntity("diana.mesh");
  SceneNode* nodeAim = _sceneMgr->createSceneNode("Target"+StringConverter::toString(_numEntities)+"SN");
  nodeAim->attachObject(entAim);
  _sceneMgr->getRootSceneNode()->addChild(nodeAim);
  
  _targets.push_back(nodeAim);

  OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter2 = new 
  OgreBulletCollisions::StaticMeshToShapeConverter(entAim);
 
  OgreBulletCollisions::TriangleMeshCollisionShape *Trimesh2 = trimeshConverter2->createTrimesh();

  OgreBulletDynamics::RigidBody *rigidObject2 = new 
    OgreBulletDynamics::RigidBody("RigidBodydiana"+StringConverter::toString(_numEntities), _world);
  rigidObject2->setShape(nodeAim, Trimesh2, 0.5, 0.5, 0,pos,//4º Posicion inicial 
       rot); //El 5º parametro es la gravedad

  delete trimeshConverter2;

  _numEntities++;

  // Anadimos los objetos a las deques---
  _shapes.push_back(Trimesh2);   
  _bodies.push_back(rigidObject2); 
  //-------------------------------------
  

}

void 
PlayState::AddDynamicObject() {
  _timeLastObject = 0.25;   // Segundos para anadir uno nuevo... 

  Vector3 position = (_camera->getDerivedPosition() + _camera->getDerivedDirection().normalisedCopy() * 10);
 
  Entity *entity = _sceneMgr->createEntity("Arrow" + StringConverter::toString(_numEntities), "arrow.mesh");
  SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode("Arrow" + StringConverter::toString(_numEntities)+"SN");
  node->attachObject(entity);

  //Obtengo la rotacion de la camara para lanzar la flecha correctamente--------------------
  Vector3 aux=_camera->getDerivedDirection().normalisedCopy(); //Este es el vector direccion
  Vector3 src = node->getOrientation() * Ogre::Vector3::UNIT_X;//Punto donde aparece la flecha
  Quaternion quat = src.getRotationTo(aux);//Rotacion
  //------------------------------------------------------------------------------------------
  
  //Colision por Box----------------------------------------------
  OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = NULL;
  OgreBulletCollisions::CollisionShape *boxShape = NULL;
  trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
  
  AxisAlignedBox boundingB = entity->getBoundingBox();
  Vector3 size = Vector3(2.0, 0.15, 0.15);//Hacer mas pequeña
  boxShape = new OgreBulletCollisions::BoxCollisionShape(size);

  OgreBulletDynamics::RigidBody *rigidBox = new OgreBulletDynamics::RigidBody("rigidBodyArrow" + StringConverter::toString(_numEntities), _world);
  
  rigidBox->setShape(node, boxShape,0.8 , 0.5 ,50.0 , position ,quat );

  //Compruebo que a fuerza no sea mas que la fuerza maxima permitida---
  if(_force>_maxForce){
    _force=_maxForce;
  }
  //-------------------------------------------------------------------

  rigidBox->setLinearVelocity(_camera->getDerivedDirection().normalisedCopy() * _force); 

  _numEntities++;

  // Anadimos los objetos a las deques---
  _shapes.push_back(boxShape);   
  _bodies.push_back(rigidBox); 
  //-------------------------------------
   

}

void 
PlayState::AddSuperArrow() {
  _timeLastObject = 0.25;   // Segundos para anadir uno nuevo... 

  Vector3 position = (_camera->getDerivedPosition() + _camera->getDerivedDirection().normalisedCopy() * 10);
 
  Entity *entity = _sceneMgr->createEntity("Arrow" + StringConverter::toString(_numEntities), "arrow.mesh");
  SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode("Arrow" + StringConverter::toString(_numEntities)+"SN");
  node->attachObject(entity);

  //Obtengo la rotacion de la camara para lanzar la flecha correctamente--------------------
  Vector3 aux=_camera->getDerivedDirection().normalisedCopy(); //Este es el vector direccion
  Vector3 src = node->getOrientation() * Ogre::Vector3::UNIT_X;//Punto donde aparece la flecha
  Quaternion quat = src.getRotationTo(aux);//Rotacion
  //------------------------------------------------------------------------------------------
  
  //Colision por Box----------------------------------------------
  OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = NULL;
  OgreBulletCollisions::CollisionShape *boxShape = NULL;
  trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
  
  AxisAlignedBox boundingB = entity->getBoundingBox();
  Vector3 size = Vector3(2.0, 0.6, 0.6);//Hacer mas pequeña
  boxShape = new OgreBulletCollisions::BoxCollisionShape(size);

  OgreBulletDynamics::RigidBody *rigidBox = new OgreBulletDynamics::RigidBody("rigidBodyArrow" + StringConverter::toString(_numEntities), _world);
  
  rigidBox->setShape(node, boxShape,0.8 , 0.5 ,50.0 , position ,quat );

  //Compruebo que a fuerza no sea mas que la fuerza maxima permitida---
  if(_force>_maxForce){
    _force=_maxForce;
  }
  //-------------------------------------------------------------------

  rigidBox->setLinearVelocity(_camera->getDerivedDirection().normalisedCopy() * _maxForce); 

  _numEntities++;

  // Anadimos los objetos a las deques---
  _shapes.push_back(boxShape);   
  _bodies.push_back(rigidBox); 
  //-------------------------------------
   

}

bool
PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{ 
  _deltaT = evt.timeSinceLastFrame;

  _world->stepSimulation(_deltaT); // Actualizar simulacion Bullet
  _timeLastObject -= _deltaT;

  

  //Si estoy pulsado el raton aumento la fueza--
  if(_mousePressed && _force<_maxForce){
    _force+=_forceInc;
  }else{
    if(!_mousePressed){
      _force = 0.0;
    }
    
  }
  //--------------------------------------------

  //Actualizo la posicion de la flecha inicial----
  Vector3 position = (_camera->getDerivedPosition() + _camera->getDerivedDirection().normalisedCopy() * 10);
  Vector3 to = _camera->getDerivedDirection().normalisedCopy(); //Este es el vector direccion
  Vector3 up = _camera->getUp().normalisedCopy();
  Vector3 aux = up.crossProduct(to).normalisedCopy();
  Quaternion quat = Quaternion(to,aux,up);//Rotacion

  if(_inGame){
    _arrowI->setPosition(position);
    _arrowI->setOrientation(quat);
  }
  
  //----------------------------------------------

  //Deteccion Colisones--------------------
  DetectCollisionAim();
  //----------------------------------------

  //Actualizo GUI-------------------
  updateGUI();
  //--------------------------------

  //Power---------------------------
  //cout << "_power: " << _power << endl;
  if(_power){
    if(--_superArrows>0){
      //Añado una super flecha---------------------------
      AddSuperArrow();
      cout << "Lanzada super flecha " << "\n" << endl;
      //-------------------------------------------
      if(_superArrows==1){
        _power=false;
      }
    }
  }
  //--------------------------------
  //Animaciones Arco--------------------------------
  if (_animBowNormal != NULL) {
    /*if (_animBowNormal->hasEnded()) {
      _animBowNormal->setTimePosition(0.0);
      _animBowNormal->setEnabled(false);
    }*/
    if (_animBowNormal->hasEnded()) {
      if(_animBowNormal->getAnimationName()=="tensar"){
        _animBowNormal = _sceneMgr->getEntity("entBow")->getAnimationState("tensar2"); //tensarNormal o tensarFast
        _animBowNormal->setEnabled(true);
        _animBowNormal->setLoop(true);
        _animBowNormal->setTimePosition(0.0);
      }
      if(_animBowNormal->getAnimationName()=="soltar"){
        _animBowNormal = _sceneMgr->getEntity("entBow")->getAnimationState("soltar2"); //tensarNormal o tensarFast
        _animBowNormal->setEnabled(true);
        _animBowNormal->setLoop(true);
        _animBowNormal->setTimePosition(0.0);
      }
    }
    else {
      _animBowNormal->addTime(_deltaT);
    }
  }
 //--------------------------------

  //Muevo el globo--------------------
  if(_inGame){
    SceneNode* _globe = _sceneMgr->getSceneNode("GloboSN");
    if(_upGlobe){
      _globe->translate(Vector3(0,0.1,0));
      if(--_globeUpDesp<0){
        _upGlobe=false;
      }
    }else{
      _globe->translate(Vector3(0,-0.11,0));
      if(++_globeUpDesp>899){
        _upGlobe=true;
      }
    }
  }
  
  //Auto-Reload-------------
  if(_autoReaload){
    if(_arrows<=0){
      _arrows=6;
    }
  }
  //------------------------
  //----------------------------------

  return true;
}

void PlayState::DetectCollisionAim() {
  //Colisiones------------------------------
  btCollisionWorld *bulletWorld = _world->getBulletCollisionWorld();
  int numManifolds = bulletWorld->getDispatcher()->getNumManifolds();

  for (int i=0;i<numManifolds;i++) {
    btPersistentManifold* contactManifold = 
      bulletWorld->getDispatcher()->getManifoldByIndexInternal(i);
    btCollisionObject* obA = 
      (btCollisionObject*)(contactManifold->getBody0());
    btCollisionObject* obB = 
      (btCollisionObject*)(contactManifold->getBody1());
    
    //Compruebo con todas dianas-----------------------------
    for(unsigned int i=0;i<_targets.size();i++){
      Ogre::SceneNode* drain = _targets[i];

      OgreBulletCollisions::Object *obDrain = _world->findObject(drain);
      OgreBulletCollisions::Object *obOB_A = _world->findObject(obA);
      OgreBulletCollisions::Object *obOB_B = _world->findObject(obB);

      if ((obOB_A == obDrain) || (obOB_B == obDrain)) {
        Ogre::SceneNode* node = NULL;
        if ((obOB_A != obDrain) && (obOB_A)) {
          node = obOB_A->getRootNode(); delete obOB_A;
        }
        else if ((obOB_B != obDrain) && (obOB_B)) {
          node = obOB_B->getRootNode(); delete obOB_B;
        }
        if (node) {
          cout << "Nodo que colisiona: " << node->getName() << "\n" << endl;
          
          //Creo una flecha en la misma posicion y rotacion---
          Entity *ent = _sceneMgr->createEntity("Arrow" + StringConverter::toString(_numEntities), "arrow.mesh");
          SceneNode *nod = _sceneMgr->getRootSceneNode()->createChildSceneNode("Arrow" + StringConverter::toString(_numEntities)+"SN");
          nod->attachObject(ent);
          nod->setPosition(node->getPosition());
          nod->setOrientation(node->getOrientation());
          _numEntities++;
          //--------------------------------------------------
          
          //Aumento la puntuacion-----------------------------
          _punt+=100;
          //-------------------------------------------------
          
          //Muestro lbl de impacto---------------------------
          _impact=true;
          //-------------------------------------------------

          //Cargo y reproduzco el sonido---
          GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("ArrowImpact.wav");
          GameManager::getSingletonPtr()->_simpleEffect->play();
          //-------------------------------
          _sceneMgr->getRootSceneNode()->removeAndDestroyChild (node->getName());
        }
      }
    }
    //------------------------------------------------

     
  }
  //----------------------------------------  
}
bool
PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{

  _deltaT = evt.timeSinceLastFrame;
  _world->stepSimulation(_deltaT); // Actualizar simulacion Bullet

  //Actualizacion de la camara--------------
  updateCameraPosition();
  //----------------------------------------s

  //Salir del juego--
  if (_exitGame)
    return false;
  //-----------------
  return true;
}

void
PlayState::keyPressed
(const OIS::KeyEvent &e)
{
  
  //SuperPower-------
  if (e.key == OIS::KC_SPACE) {
    if(_power==false){
      _power=true;
      CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
      sheet->getChild("imgPower")->setVisible(false);
    }
    
  }
  //-----------------

  // Tecla p --> PauseState.-------
  if (e.key == OIS::KC_P) {
    pushState(PauseState::getSingletonPtr());
  }
  //-----------------

  // Tecla p --> GameOverState.-------
  if (e.key == OIS::KC_G) {
    GameManager::getSingletonPtr()->setPunt(_punt);
    changeState(GameOverState::getSingletonPtr());
  }
  //-----------------

  //Movimiento camara---------------
  Vector3 vt(0,0,0);
  Real tSpeed = 20.0;
  if (e.key == OIS::KC_UP) {
    vt+=Vector3(0,0,-_desp);
  }
  if (e.key == OIS::KC_DOWN) {
    vt+=Vector3(0,0,_desp);
  }
  if (e.key == OIS::KC_LEFT) {
    vt+=Vector3(-_desp,0,0);
  }
  if (e.key == OIS::KC_RIGHT) {
    vt+=Vector3(_desp,0,0);
  }
  _camera->moveRelative(vt * _deltaT * tSpeed *_desp);
  //-------------------------------- 


  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);
  //-------------------------------
}

void
PlayState::keyReleased
(const OIS::KeyEvent &e)
{
  
  //Recargo flechas---------------
  if (e.key == OIS::KC_R) {
    _arrows=6;
  }
  //------------------------------

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
PlayState::mouseMoved
(const OIS::MouseEvent &e)
{

  //Movimiento camara-----------------------------------------------------------------------------
  float rotx = e.state.X.rel * _deltaT * -1;
  float roty = e.state.Y.rel * _deltaT * -1;
  _camera->yaw(Radian(rotx));
  _camera->pitch(Radian(roty));
  //----------------------------------------------------------------------------------------------

  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);
  //-------------------------------
}

void
PlayState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
  //-------------------------------
  
  //Pongo el raton a pulsado---
  _mousePressed=true;
  //---------------------------

  //Animacion tensar-----------------------------------------------------------
  _animBowNormal = _sceneMgr->getEntity("entBow")->getAnimationState("tensar"); //tensarNormal o tensarFast
  _animBowNormal->setEnabled(true);
  _animBowNormal->setLoop(false);
  _animBowNormal->setTimePosition(0.0);
  //---------------------------------------------------------------------------
  
}

void
PlayState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
  //-------------------------------

  //Pongo el raton a levantado y disparo---
  //Si me quedan flechas disparo-----------
  if(_arrows>0){
    //Cargo y reproduzco el sonido---
    GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("Shot.wav");
    GameManager::getSingletonPtr()->_simpleEffect->play();
    //-------------------------------
    AddDynamicObject();
    --_arrows;
  }
  //---------------------------------------  
  _mousePressed=false;
  //---------------------------------------

  //Animacion soltar-----------------------------------------------------------
  _animBowNormal = _sceneMgr->getEntity("entBow")->getAnimationState("soltar"); //tensarNormal o tensarFast
  _animBowNormal->setEnabled(true);
  _animBowNormal->setLoop(false);
  _animBowNormal->setTimePosition(0.0);
  //---------------------------------------------------------------------------
}

PlayState*
PlayState::getSingletonPtr ()
{
return msSingleton;
}

PlayState&
PlayState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

CEGUI::MouseButton PlayState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
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
PlayState::quit(const CEGUI::EventArgs &e)
{
  _exitGame=true;
  return true;
}

void
PlayState::updateCameraPosition()
{
  //Actualizacion de la posicion---------------------------
  //cout << "Pos Camara: " << _camera->getPosition() << endl;
  _camera->setPosition(_camera->getPosition()+_vnCam*_speedCam*_deltaT);
  //-------------------------------------------------------

  //Comprobacion de punto----------------------------------
  int _distance=_next.distance(_camera->getPosition());
  if(_distance==0){
    _now=_next;
    _next=Vector3(-1,-1,-1);//COMO SI FUERA NULL, pero no se puede usar null
  }
  //-------------------------------------------------------

  //Siguiente Punto--------------------------------------
  if(_next==Vector3(-1,-1,-1)){
    if(_points.size()>0){
      _next=_points.front();
      _points.erase(_points.begin());
      Vector3 _aux = _next-_now;
      _vnCam= _aux.normalisedCopy();
    }else{//Hemos llegado al ultimo punto (Se acabaria el recorrido )
      cout << "Fin Recorrido\n" << endl;
      _speedCam=0;
      _inGame=false;
      _points.clear();
      //Paso a Game Over----------------
      GameManager::getSingletonPtr()->setPunt(_punt);
      changeState(GameOverState::getSingletonPtr());
      //--------------------------------
    }
  }
}

void
PlayState::createGUI()
{
  // Interfaz Intro--------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

  //Botones Juego---------------------------------------
  CEGUI::Window* quitButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","quitButton2");
  quitButton->setText("[font='DickVanDyke'] Exit");
  quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.05,0)));
  quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.80,0),CEGUI::UDim(0.04,0)));
  quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::quit,this));

  CEGUI::Window* pauseButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","pauseButton");
  pauseButton->setText("[font='DickVanDyke'] Pause");
  pauseButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.05,0)));
  pauseButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.60,0),CEGUI::UDim(0.04,0)));
  pauseButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::pauseB,this));
  //-------------------------------------------------------
  
  //Label Puntuacion-------------------------------------------
  CEGUI::Window* textPoints = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textPoints");
  textPoints->setText("[font='DickVanDyke'] 000");
  textPoints->setSize(CEGUI::USize(CEGUI::UDim(0.20,0),CEGUI::UDim(0.70,0)));
  textPoints->setPosition(CEGUI::UVector2(CEGUI::UDim(0.03f, 0.0f),CEGUI::UDim(0.04f, 0.0f)));
  textPoints->setProperty("FrameEnabled","False");
  textPoints->setProperty("BackgroundEnabled", "False");
  textPoints->setProperty("VertFormatting", "TopAligned");
  //-----------------------------------------------------------

  //Label Impacto----------------------------------------------
  CEGUI::Window* textImpact = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textImpact");
  textImpact->setText("[font='DickVanDyke-26'] +100");
  textImpact->setSize(CEGUI::USize(CEGUI::UDim(0.8,0),CEGUI::UDim(0.9,0)));
  textImpact->setPosition(CEGUI::UVector2(CEGUI::UDim(0.38f, 0.0f),CEGUI::UDim(0.15f, 0.0f)));
  textImpact->setProperty("FrameEnabled","False");
  textImpact->setProperty("BackgroundEnabled", "False");
  textImpact->setProperty("VertFormatting", "TopAligned");
  textImpact->setVisible(false);
  //-----------------------------------------------------------


  CEGUI::Window* img1 = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","img1");
  img1->setPosition(CEGUI::UVector2(CEGUI::UDim(0.90f, 0.0f),CEGUI::UDim(0.80f, 0.0f)));
  img1->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.2,0)));
  img1->setProperty("Image","BackgroundImageArrow");
  img1->setProperty("FrameEnabled","False");
  img1->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* img2 = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","img2");
  img2->setPosition(CEGUI::UVector2(CEGUI::UDim(0.85f, 0.0f),CEGUI::UDim(0.80f, 0.0f)));
  img2->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.2,0)));
  img2->setProperty("Image","BackgroundImageArrow");
  img2->setProperty("FrameEnabled","False");
  img2->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* img3 = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","img3");
  img3->setPosition(CEGUI::UVector2(CEGUI::UDim(0.80f, 0.0f),CEGUI::UDim(0.80f, 0.0f)));
  img3->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.2,0)));
  img3->setProperty("Image","BackgroundImageArrow");
  img3->setProperty("FrameEnabled","False");
  img3->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* img4 = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","img4");
  img4->setPosition(CEGUI::UVector2(CEGUI::UDim(0.75f, 0.0f),CEGUI::UDim(0.80f, 0.0f)));
  img4->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.2,0)));
  img4->setProperty("Image","BackgroundImageArrow");
  img4->setProperty("FrameEnabled","False");
  img4->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* img5 = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","img5");
  img5->setPosition(CEGUI::UVector2(CEGUI::UDim(0.70f, 0.0f),CEGUI::UDim(0.80f, 0.0f)));
  img5->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.2,0)));
  img5->setProperty("Image","BackgroundImageArrow");
  img5->setProperty("FrameEnabled","False");
  img5->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* img6 = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","img6");
  img6->setPosition(CEGUI::UVector2(CEGUI::UDim(0.65f, 0.0f),CEGUI::UDim(0.80f, 0.0f)));
  img6->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.2,0)));
  img6->setProperty("Image","BackgroundImageArrow");
  img6->setProperty("FrameEnabled","False");
  img6->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* imgPower = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","imgPower");
  imgPower->setPosition(CEGUI::UVector2(CEGUI::UDim(0.30f, 0.0f),CEGUI::UDim(0.80f, 0.0f)));
  imgPower->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.2,0)));
  imgPower->setProperty("Image","BackgroundImagePower");
  imgPower->setProperty("FrameEnabled","False");
  imgPower->setProperty("BackgroundEnabled", "False");

  
  CEGUI::Window* bar = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","bar");
  bar->setPosition(CEGUI::UVector2(CEGUI::UDim(0.79f, 0.0f),CEGUI::UDim(0.60f, 0.0f)));
  bar->setSize(CEGUI::USize(CEGUI::UDim(0.07,0),CEGUI::UDim(0.03,0)));
  bar->setProperty("Image","BackgroundImageBar");
  bar->setProperty("FrameEnabled","False");
  bar->setProperty("BackgroundEnabled", "False");


  CEGUI::Window* meter = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","meter");
  meter->setPosition(CEGUI::UVector2(CEGUI::UDim(0.79f, 0.0f),CEGUI::UDim(0.31f, 0.0f)));
  meter->setSize(CEGUI::USize(CEGUI::UDim(0.07,0),CEGUI::UDim(0.3,0)));
  meter->setProperty("Image","BackgroundImageMeter");
  meter->setProperty("FrameEnabled","False");
  meter->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* textR = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textR");
  textR->setText("[font='DickVanDyke-26'] Reload!!!");
  textR->setSize(CEGUI::USize(CEGUI::UDim(0.8,0),CEGUI::UDim(0.9,0)));
  textR->setPosition(CEGUI::UVector2(CEGUI::UDim(0.30f, 0.0f),CEGUI::UDim(0.15f, 0.0f)));
  textR->setProperty("FrameEnabled","False");
  textR->setProperty("BackgroundEnabled", "False");
  textR->setProperty("VertFormatting", "TopAligned");
  textR->setVisible(false);

  sheet->addChild(textR);
  sheet->addChild(meter);
  sheet->addChild(bar);
  sheet->addChild(img1);
  sheet->addChild(img2);
  sheet->addChild(img3);
  sheet->addChild(img4);
  sheet->addChild(img5);
  sheet->addChild(img6);
  sheet->addChild(imgPower);
  sheet->addChild(textPoints);
  sheet->addChild(textImpact);
  sheet->addChild(quitButton);
  sheet->addChild(pauseButton);
  //------------------------------
}
bool
PlayState::pauseB(const CEGUI::EventArgs &e)
{
  pushState(PauseState::getSingletonPtr());
  return true;
}


void
PlayState::updateGUI()
{

  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  sheet->getChild("textPoints")->setText("[font='DickVanDyke']  Score: "+Ogre::StringConverter::toString(_punt));

  if(_impact){
    sheet->getChild("textImpact")->setVisible(true);
    if(--_nFramesGUIImpact<0){
      _nFramesGUIImpact=120;
      sheet->getChild("textImpact")->setVisible(false);
      _impact=false;
    }
  }


  //Barra de fuerza------------------------------------------------------------
  sheet->getChild("bar")->setPosition(CEGUI::UVector2(CEGUI::UDim(0.79f, 0.0f),
     	CEGUI::UDim(0.60f, 0.0f)-CEGUI::UDim(0.03f*_force*0.05, 0.0f)));
  //---------------------------------------------------------------------------

  //Flechas----------------------------------------------------------

  switch(_arrows){
    case 6:
      sheet->getChild("img6")->setVisible(true);
      sheet->getChild("img5")->setVisible(true);
      sheet->getChild("img4")->setVisible(true);
      sheet->getChild("img3")->setVisible(true);
      sheet->getChild("img2")->setVisible(true);
      sheet->getChild("img1")->setVisible(true);
      sheet->getChild("textR")->setVisible(false);
      break;
    case 5:
      sheet->getChild("img6")->setVisible(false);
      sheet->getChild("img5")->setVisible(true);
      sheet->getChild("img4")->setVisible(true);
      sheet->getChild("img3")->setVisible(true);
      sheet->getChild("img2")->setVisible(true);
      sheet->getChild("img1")->setVisible(true);
      sheet->getChild("textR")->setVisible(false);
      break;
    case 4:
      sheet->getChild("img6")->setVisible(false);
      sheet->getChild("img5")->setVisible(false);
      sheet->getChild("img4")->setVisible(true);
      sheet->getChild("img3")->setVisible(true);
      sheet->getChild("img2")->setVisible(true);
      sheet->getChild("img1")->setVisible(true);
      sheet->getChild("textR")->setVisible(false);
      break;
    case 3:
      sheet->getChild("img6")->setVisible(false);
      sheet->getChild("img5")->setVisible(false);
      sheet->getChild("img4")->setVisible(false);
      sheet->getChild("img3")->setVisible(true);
      sheet->getChild("img2")->setVisible(true);
      sheet->getChild("img1")->setVisible(true);
      sheet->getChild("textR")->setVisible(false);
      break;
    case 2:
      sheet->getChild("img6")->setVisible(false);
      sheet->getChild("img5")->setVisible(false);
      sheet->getChild("img4")->setVisible(false);
      sheet->getChild("img3")->setVisible(false);
      sheet->getChild("img2")->setVisible(true);
      sheet->getChild("img1")->setVisible(true);
      sheet->getChild("textR")->setVisible(false);
      break;
    case 1:
      sheet->getChild("img6")->setVisible(false);
      sheet->getChild("img5")->setVisible(false);
      sheet->getChild("img4")->setVisible(false);
      sheet->getChild("img3")->setVisible(false);
      sheet->getChild("img2")->setVisible(false);
      sheet->getChild("img1")->setVisible(true);
      sheet->getChild("textR")->setVisible(false);
      break;
    case 0:
      sheet->getChild("img6")->setVisible(false);
      sheet->getChild("img5")->setVisible(false);
      sheet->getChild("img4")->setVisible(false);
      sheet->getChild("img3")->setVisible(false);
      sheet->getChild("img2")->setVisible(false);
      sheet->getChild("img1")->setVisible(false);
      sheet->getChild("textR")->setVisible(true);
      break;
  }
  //-----------------------------------------------------------------
}

void
PlayState::loadConfig()
{
  fstream fichero;//Fichero
  std::string frase;//Auxiliar

  fichero.open( "data/Config.txt" , ios::in);
  if (fichero.is_open()) {
    cout << "===CONFIGURACION===" << endl;
    while (getline (fichero,frase)) {
      string _option=Ogre::StringUtil::split(frase,"=")[0];
      string _value=Ogre::StringUtil::split(frase,"=")[1];
      
      //Opciones de configuracion-----------------------------
      if(Ogre::StringUtil::startsWith(_option,"MouseSpeed")){
        _desp=Ogre::StringConverter::parseInt(_value);
        if(_desp<5){
          _desp=5;
        }
        cout << "Desp: " << _desp << endl;
      }

      if(Ogre::StringUtil::startsWith(_option,"AutoReaload")){
        Ogre::StringUtil::trim(_value);
        cout << "Value: " << _value << endl;
        if(Ogre::StringUtil::startsWith(_value,"true")){
          _autoReaload=true;
        }else{
          _autoReaload=false;
        }
        cout << "AutoReaload: " << _autoReaload << endl;
      }
      //-------------------------------------------------------

    }
    fichero.close();
    cout << "===================" << endl; 
  }else{
    //Si no existe el fichero de configuracion creo uno por defecto---
    cout << "Cargando configuracion por defecto" << endl;
    ofstream archivo;  // objeto de la clase ofstream
    archivo.open("data/Config.txt");
    archivo<<"MouseSpeed = 20"<< endl;
    archivo<<"AutoReaload = false"<< endl;
    archivo.close();
    //----------------------------------------------------------------
    //Vuelvo a leer el fichero de configuracion---
    loadConfig();
    //--------------------------------------------
  }
}