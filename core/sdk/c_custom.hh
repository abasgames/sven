#pragma once

namespace sdk {
	typedef enum
	{
		t_sound = 0,
		t_skin,
		t_model,
		t_decal,
		t_generic,
		t_eventscript,
		t_world, // Fake type for world, is really t_model
	} resourcetype_t;


	typedef struct {
		int size;
	}_resourceinfo_t;

	typedef struct resourceinfo_s {
		_resourceinfo_t info[8];
	}resourceinfo_t;

	typedef struct resource_s {
		char szFileName[ 64 ]; // File name to download/precache.
		resourcetype_t type;		// t_sound, t_skin, t_model, t_decal.
		int nIndex;					// For t_decals
		int nDownloadSize;			// Size in Bytes if this must be downloaded.
		unsigned char ucFlags;

		// For handling client to client resource propagation
		unsigned char rgucMD5_hash[16]; // To determine if we already have it.
		unsigned char playernum;		// Which player index this resource is associated with, if it's a custom resource.

		unsigned char rguc_reserved[32]; // For future expansion
		struct resource_s* pNext;		 // Next in chain.
		struct resource_s* pPrev;
	} resource_t;

	typedef struct customization_s {
		int bInUse;			   // Is this customization in use;
		resource_t resource;		   // The resource_t for this customization
		int bTranslated;		   // Has the raw data been translated into a useable format?
									   //  (e.g., raw decal .wad make into texture_t *)
		int nUserData1;				   // Customization specific data
		int nUserData2;				   // Customization specific data
		void* pInfo;				   // Buffer that holds the data structure that references the data (e.g., the cachewad_t)
		void* pBuffer;				   // Buffer that holds the data for the customization (the raw .wad data)
		struct customization_s* pNext; // Next in chain
	} customization_t;
};