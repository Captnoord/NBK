#ifndef SETTINGS_H
#define SETTINGS_H

namespace game_utils
{
	// settings file param names
	#define CV_SETTINGS_FLUID_ANIMATION_SPEED			"ST_FLUID_ANIMATION_SPEED"
	#define CV_SETTINGS_FLUID_ANIMATION_MAX_AMPLITUDE	"ST_FLUID_ANIMATION_MAX_AMPLITUDE"

	#define CV_SETTINGS_CAMERA_INITIAL_X_ROTATION		"ST_CAMERA_INITIAL_X_ROTATION"
	#define CV_SETTINGS_CAMERA_ROTATE_Y_SPEED			"ST_CAMERA_ROTATE_Y_SPEED"
	#define CV_SETTINGS_CAMERA_ZOOM_SPEED				"ST_CAMERA_ZOOM_SPEED"
	#define CV_SETTINGS_CAMERA_MOVE_SPEED				"ST_CAMERA_MOVE_SPEED"
	#define CV_SETTINGS_CAMERA_FPS_EXTENT				"ST_CAMERA_FPS_EXTENT"
	#define CV_SETTINGS_CAMERA_MLOOK_SPEED				"ST_CAMERA_MLOOK_SPEED"

	// texture atlas info
	#define CV_SETTINGS_TEXTURE_ATLAS_WIDTH				"ST_TEXTURE_ATLAS_WIDTH"
	#define CV_SETTINGS_TEXTURE_ATLAS_HEIGHT			"ST_TEXTURE_ATLAS_HEIGHT"
	#define CV_SETTINGS_TEXTURE_ATLAS_SUBTILE_SIZE		"ST_TEXTURE_ATLAS_SUBTILE_SIZE"
	#define CV_SETTINGS_TEXTURE_ATLAS_PIXEL_WIDTH		"ST_TEXTURE_ATLAS_PIXEL_WIDTH"
	#define CV_SETTINGS_TEXTURE_ATLAS_PIXEL_HEIGHT		"ST_TEXTURE_ATLAS_PIXEL_HEIGHT"
	#define CV_SETTINGS_TEXTURE_QUALITY					"ST_TEXTURE_QUALITY"
	#define CV_SETTINGS_TEXTURE_ATLAS_INDEX				"ST_TEXTURE_ATLAS_INDEX"

	#define CV_SETTINGS_USE_PPL							"ST_USE_PPL"
};

#endif // SETTINGS_H