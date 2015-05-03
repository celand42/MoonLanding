AutomatedMakefile = am
CC = g++ 

GAME_PATH=/$(GAME_DRIVE_LETTER)/Game_Engine

INC_DIRS=-I./ -I$(GAME_PATH)/Ogre/OgreMain/include/ -I$(GAME_PATH)/Ogre/include/ -I$(GAME_PATH)/boost_1_57_0/ -I$(GAME_PATH)/tinyxml -I$(GAME_PATH)/OgreDeps/src/ois/includes -I$(GAME_PATH)/bass_2.4 -I$(GAME_PATH)/MyGUI_3.2.2/MyGUIEngine/include -I$(GAME_PATH)/MyGUI_3.2.2/Platforms/Ogre/OgrePlatform/include -I$(GAME_PATH)/lua_5.3.0/install -I$(GAME_PATH)/bullet/src -I$(GAME_PATH)/poco_1.4.6/Net/include -I$(GAME_PATH)/poco_1.4.6/Foundation/include -I$(GAME_PATH)/poco_1.4.6/Util/include -I$(GAME_PATH)/random -I$(GAME_PATH)
LIB_DIRS=-L./ -L$(GAME_PATH)/Ogre/lib/ -L$(GAME_PATH)/boost_1_57_0/stage/lib/ -L$(GAME_PATH)/tinyxml -L$(GAME_PATH)/OgreDeps/src/ois -L$(GAME_PATH)/bass_2.4 -L$(GAME_PATH)/MyGUI_3.2.2/lib -L$(GAME_PATH)/lua_5.3.0/install/lib -L$(GAME_PATH)/bullet/lib -L$(GAME_PATH)/poco_1.4.6/lib/MinGW/ia32 -L$(GAME_PATH)/random
LIBS=-lboost_system-mgw48-mt-1_57 -lboost_thread-mgw48-mt-1_57 -ltinyxml -lOIS -lbass -llua -lMyGUI.OgrePlatform -lMyGUIEngine -lOgreMain -lBulletDynamics -lBulletCollision -lLinearMath -lPocoNet -lPocoFoundation -lWS2_32 -lrandom

COMPILE = $(CC) $(INC_DIRS) -c 
LINK = $(CC) $(LIB_DIRS) -o

GAME_FILES = Main.o GameManager.o RenderListener.o AnimationRenderListener.o InputRenderListener.o PhysicsRenderListener.o NetworkRenderListener.o RenderManager.o LogManager.o GameResource.o PathResource.o MeshResource.o ResourceManager.o InputManager.o AudioResource.o AudioPlayer.o AudioManager.o GUIManager.o ScriptManager.o GUIWidgetScript.o PhysicsManager.o RigidBody.o BulletSceneNodeMotionState.o BulletDebugDrawer.o CompoundShape.o NetworkUtils.o NetworkManager.o Saber.o
QUAT_FILES = Quaternion.o
SERVER_FILES = GameConnection.o GameServer.o NetworkUtils.o

all:	Ogre

Ogre:   		$(GAME_FILES)
			$(LINK) Game.exe $(GAME_FILES) $(LIBS)

Quaternion:		$(QUAT_FILES)
			$(LINK) Quaternion.exe $(QUAT_FILES)

Server:			$(SERVER_FILES)
			$(LINK) GameServer.exe $(SERVER_FILES) $(LIBS)

Main.o:			Main.cpp
			$(COMPILE) Main.cpp

GameManager.o:		GameManager.h GameManager.cpp
			$(COMPILE) GameManager.cpp

RenderManager.o:	RenderManager.h RenderManager.cpp
			$(COMPILE) RenderManager.cpp

RenderListener.o:	RenderListener.h RenderListener.cpp
			$(COMPILE) RenderListener.cpp

AnimationRenderListener.o:	AnimationRenderListener.h AnimationRenderListener.cpp
				$(COMPILE) AnimationRenderListener.cpp

InputRenderListener.o:		InputRenderListener.h InputRenderListener.cpp
				$(COMPILE) InputRenderListener.cpp

PhysicsRenderListener.o:	PhysicsRenderListener.h PhysicsRenderListener.cpp
				$(COMPILE) PhysicsRenderListener.cpp

NetworkRenderListener.o:	NetworkRenderListener.h NetworkRenderListener.cpp
				$(COMPILE) NetworkRenderListener.cpp

LogManager.o:		LogManager.h LogManager.cpp
			$(COMPILE) LogManager.cpp

Quaternion.o:		Quaternion.h Quaternion.cpp
			$(COMPILE) Quaternion.cpp

GameResource.o:		GameResource.h GameResource.cpp
			$(COMPILE) GameResource.cpp

PathResource.o:		PathResource.h PathResource.cpp
			$(COMPILE) PathResource.cpp

MeshResource.o:		MeshResource.h MeshResource.cpp
			$(COMPILE) MeshResource.cpp

ResourceManager.o:	ResourceManager.h ResourceManager.cpp
			$(COMPILE) ResourceManager.cpp

InputManager.o:		InputManager.h InputManager.cpp InputListener.h
			$(COMPILE) InputManager.cpp

AudioResource.o:	AudioResource.h AudioResource.cpp
			$(COMPILE) AudioResource.cpp

AudioPlayer.o:		AudioPlayer.h AudioPlayer.cpp
			$(COMPILE) AudioPlayer.cpp

AudioManager.o:		AudioManager.h AudioManager.cpp
			$(COMPILE) AudioManager.cpp

GUIManager.o:		GUIManager.h GUIManager.cpp
			$(COMPILE) GUIManager.cpp

ScriptManager.o:	ScriptManager.h ScriptManager.cpp
			$(COMPILE) ScriptManager.cpp

GUIWidgetScript.o:	GUIWidgetScript.h GUIWidgetScript.cpp
			$(COMPILE) GUIWidgetScript.cpp

PhysicsManager.o:	PhysicsManager.h PhysicsManager.cpp
			$(COMPILE) PhysicsManager.cpp

RigidBody.o:		RigidBody.h RigidBody.cpp
			$(COMPILE) RigidBody.cpp

BulletSceneNodeMotionState.o:	BulletSceneNodeMotionState.h BulletSceneNodeMotionState.cpp
				$(COMPILE) BulletSceneNodeMotionState.cpp

BulletDebugDrawer.o:	BulletDebugDrawer.h BulletDebugDrawer.cpp
			$(COMPILE) BulletDebugDrawer.cpp

CompoundShape.o:	CompoundShape.h CompoundShape.cpp
			$(COMPILE) CompoundShape.cpp

NetworkUtils.o:		NetworkUtils.h NetworkUtils.cpp
			$(COMPILE) NetworkUtils.cpp

NetworkManager.o:	NetworkManager.h NetworkManager.cpp
			$(COMPILE) NetworkManager.cpp

GameConnection.o:	GameConnection.h GameConnection.cpp
			$(COMPILE) GameConnection.cpp

GameServer.o:		GameServer.cpp
			$(COMPILE) GameServer.cpp

Saber.o:		Saber.cpp
			$(COMPILE) Saber.cpp

# AIManager.o:		AIManager.h AIManager.cpp
# 			$(COMPILE) AIManager.cpp


