/*********************************************************************
 * Módulo 1. Curso de Experto en Desarrollo de Videojuegos
 * Autor: David Vallejo Fernández    David.Vallejo@uclm.es
 *
 * Código modificado a partir de Managing Game States with OGRE
 * http://www.ogre3d.org/tikiwiki/Managing+Game+States+with+OGRE
 * Inspirado en Managing Game States in C++
 * http://gamedevgeek.com/tutorials/managing-game-states-in-c/
 *
 * You can redistribute and/or modify this file under the terms of the
 * GNU General Public License ad published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * and later version. See <http://www.gnu.org/licenses/>.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  
 *********************************************************************/

#ifndef IntroState_H
#define IntroState_H

#include <Ogre.h>
#include <OIS/OIS.h>

#include "GameState.h"
#include <CEGUI.h>
 #include <RendererModules/Ogre/Renderer.h>

class IntroState : public Ogre::Singleton<IntroState>, public GameState
{
 public:
  IntroState() {}

  void enter ();
  void exit ();
  void pause ();
  void resume ();

  void keyPressed (const OIS::KeyEvent &e);
  void keyReleased (const OIS::KeyEvent &e);

  void mouseMoved (const OIS::MouseEvent &e);
  void mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id);
  void mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id);

  bool frameStarted (const Ogre::FrameEvent& evt);
  bool frameEnded (const Ogre::FrameEvent& evt);

  // Heredados de Ogre::Singleton.
  static IntroState& getSingleton ();
  static IntroState* getSingletonPtr ();

  void createGUI();
  bool play(const CEGUI::EventArgs &e);
  bool credits(const CEGUI::EventArgs &e);
  bool records(const CEGUI::EventArgs &e);
  bool back(const CEGUI::EventArgs &e);
  bool quit(const CEGUI::EventArgs &e);
  bool info(const CEGUI::EventArgs &e);
  bool options(const CEGUI::EventArgs &e);
  CEGUI::MouseButton convertMouseButton(OIS::MouseButtonID id);
  bool onSliderValueChanged(const CEGUI::EventArgs &e);
  bool apply(const CEGUI::EventArgs &e);
  bool resetRecords(const CEGUI::EventArgs &e);
  bool resetConfig(const CEGUI::EventArgs &e);
  bool tbFullscreenChanged(const CEGUI::EventArgs &e);
  bool changeResolution(const CEGUI::EventArgs &e);

  void createRecordsFile();
  std::string readRecords();

 protected:
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneMgr;
  Ogre::Viewport* _viewport;
  Ogre::Camera* _camera;

  CEGUI::OgreRenderer* renderer;

  bool _exitGame;
  bool _credits;
  bool _options;
  bool _info;
  bool _fullscreen;
  int _resWidth;
  int _resHeigt;
};

#endif
