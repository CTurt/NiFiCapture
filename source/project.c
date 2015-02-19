#include "DSGM.h"

#include "DSGM_projectHelper.h"

// User variables / declarations
int packets = 0;

// Add custom rules to ignore packets from other devices
bool validPacket(void) {
	bool valid = true;
	
	if(DSGM_wirelessDataLength > 0x06) {
		if(strncmp(DSGM_wirelessData + 0x06, "BTWiFi", 6) == 0) valid = false;
		if(strncmp(DSGM_wirelessData + 0x06, "BTHomeHub", 9) == 0) valid = false;
	}
	
	return valid;
}

// Resources
DSGM_Sound DSGM_Sounds[DSGM_SOUND_COUNT] = {
};

DSGM_Background DSGM_Backgrounds[DSGM_BACKGROUND_COUNT] = {
};

DSGM_Palette DSGM_Palettes[DSGM_PALETTE_COUNT] = {
};

DSGM_Sprite DSGM_Sprites[DSGM_SPRITE_COUNT] = {
};

DSGM_Object DSGM_Objects[DSGM_OBJECT_COUNT] = {
	// hello
	{
		DSGM_NO_SPRITE,
		(DSGM_Event)hello_create,
		(DSGM_Event)hello_loop,
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((helloObjectInstance *)0)->variables)
	},
};

DSGM_Room DSGM_Rooms[DSGM_ROOM_COUNT] = {
	// Room_1
	{
		// Backgrounds
		{
			// Bottom screen
			{
				// Layer 0
				{
					DSGM_DEFAULT_FONT,			// Background
					DSGM_BOTTOM,				// Screen
					0,							// Layer
					false,						// Attached to view system
					7,							// Map base
					0,							// Tile base
					0, 0, 0
				},
				
				// Layer 1
				{
					DSGM_NO_BACKGROUND,			// Background
					DSGM_BOTTOM,				// Screen
					1,							// Layer
					true,						// Attached to view system
					0,							// Map base
					0,							// Tile base
					0, 0, 0
				},
				
				// Layer 2
				{
					DSGM_NO_BACKGROUND,			// Background
					DSGM_BOTTOM,				// Screen
					2,							// Layer
					true,						// Attached to view system
					0,							// Map base
					0,							// Tile base
					0, 0, 0
				},
				
				// Layer 3
				{
					DSGM_NO_BACKGROUND,			// Background
					DSGM_BOTTOM,				// Screen
					3,							// Layer
					true,						// Attached to view system
					0,							// Map base
					0,							// Tile base
					0, 0, 0
				},
			},
			
			// Top screen
			{
				// Layer 0
				{
					DSGM_DEFAULT_FONT,			// Background
					DSGM_TOP,					// Screen
					0,							// Layer
					false,						// Attached to view system
					7,							// Map base
					0,							// Tile base
					0, 0, 0
				},
				
				// Layer 1
				{
					DSGM_NO_BACKGROUND,			// Background
					DSGM_TOP,					// Screen
					1,							// Layer
					true,						// Attached to view system
					0,							// Map base
					0,							// Tile base
					0, 0, 0
				},
				
				// Layer 2
				{
					DSGM_NO_BACKGROUND,			// Background
					DSGM_TOP,					// Screen
					2,							// Layer
					true,						// Attached to view system
					0,							// Map base
					0,							// Tile base
					0, 0, 0
				},
				
				// Layer 3
				{
					DSGM_NO_BACKGROUND,			// Background
					DSGM_TOP,					// Screen
					3,							// Layer
					true,						// Attached to view system
					0,							// Map base
					0,							// Tile base
					0, 0, 0
				},
			}
		},
		
		// Initial views
		{
			// Bottom screen
			{
				0, 0
			},
			
			// Top screen
			{
				0, 0
			}
		},
		
		// Views
		{
			// Bottom screen
			{
				0, 0
			},
			
			// Top screen
			{
				0, 0
			}
		},
		
		NULL,
		
		// Object groups are dynamic, so must be set up at run time, see DSGM_SetupRooms.
		{
			NULL,
			NULL
		},
		{
			0,
			0
		}
	},
};

int DSGM_currentRoom = Room_1;

void DSGM_SetupRooms(int room) {
	if(room != DSGM_ALL_ROOMS) {
		switch(room) {
			case Room_1: goto Room_1; break;
		}
	}
	
	Room_1:
	DSGM_Debug("Room_1 reset\n");
	DSGM_LeaveRoom(&DSGM_Rooms[Room_1]);
	
	DSGM_SetupViews(&DSGM_Rooms[Room_1]);
	
	DSGM_SetupObjectGroups(&DSGM_Rooms[Room_1], DSGM_TOP, 0);
	
	DSGM_SetupObjectGroups(&DSGM_Rooms[Room_1], DSGM_BOTTOM, 1);
	
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][0], &DSGM_Objects[hello], DSGM_BOTTOM, 1,
		0, 0
	);
	
	if(room != DSGM_ALL_ROOMS) return;
}

void hello_create(helloObjectInstance *me) {
	DSGM_InitNiFi();
	
	mkdir("fat:/packets", 0777);
	
	DSGM_DrawText(DSGM_BOTTOM, 1, 1, "CTurt's NiFi capture tool");
	DSGM_DrawText(DSGM_BOTTOM, 1, 3, "Press B to end");
	DSGM_DrawText(DSGM_BOTTOM, 1, 5, "Capturing...");
}

void hello_loop(helloObjectInstance *me) {
	DSGM_DrawText(DSGM_BOTTOM, 1, 7, "Packets: %d\n", packets);
	
	if(DSGM_newWirelessData && validPacket()) {
		packets++;
		
		char filename[32];
		sprintf(filename, "fat:/packets/packet-%d.bin", packets);
		
		FILE *f = NULL;
		f = fopen(filename, "wb");
		if(!f) {
			DSGM_DisableWireless();
			DSGM_DrawText(DSGM_BOTTOM, 1, 5, "Finished!   ");
			DSGM_DrawText(DSGM_BOTTOM, 1, 9, "Could not write packet!");
		}
		
		fwrite(DSGM_wirelessData, 1, DSGM_wirelessDataLength, f);
		fclose(f);
	}
	
	//DSGM_SendWirelessData(data, dataLength);
	
	if(DSGM_newpress.B) {
		DSGM_DisableWireless();
		DSGM_DrawText(DSGM_BOTTOM, 1, 5, "Finished!   ");
	}
}
