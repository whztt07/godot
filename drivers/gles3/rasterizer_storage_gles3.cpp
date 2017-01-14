#include "rasterizer_storage_gles3.h"
#include "rasterizer_canvas_gles3.h"
#include "rasterizer_scene_gles3.h"
#include "globals.h"

/* TEXTURE API */

#define _EXT_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                   0x8C00
#define _EXT_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                   0x8C01
#define _EXT_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                  0x8C02
#define _EXT_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                  0x8C03

#define _EXT_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT               0x8A54
#define _EXT_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT               0x8A55
#define _EXT_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT         0x8A56
#define _EXT_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT         0x8A57


#define _EXT_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define _EXT_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define _EXT_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3

#define _EXT_COMPRESSED_LUMINANCE_LATC1_EXT                 0x8C70
#define _EXT_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT          0x8C71
#define _EXT_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT           0x8C72
#define _EXT_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT    0x8C73


#define _EXT_COMPRESSED_RED_RGTC1_EXT 0x8DBB
#define _EXT_COMPRESSED_RED_RGTC1 0x8DBB
#define _EXT_COMPRESSED_SIGNED_RED_RGTC1 0x8DBC
#define _EXT_COMPRESSED_RG_RGTC2 0x8DBD
#define _EXT_COMPRESSED_SIGNED_RG_RGTC2 0x8DBE
#define _EXT_COMPRESSED_SIGNED_RED_RGTC1_EXT 0x8DBC
#define _EXT_COMPRESSED_RED_GREEN_RGTC2_EXT 0x8DBD
#define _EXT_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT 0x8DBE
#define _EXT_ETC1_RGB8_OES           0x8D64



#define _EXT_SLUMINANCE_NV                                  0x8C46
#define _EXT_SLUMINANCE_ALPHA_NV                            0x8C44
#define _EXT_SRGB8_NV                                       0x8C41
#define _EXT_SLUMINANCE8_NV                                 0x8C47
#define _EXT_SLUMINANCE8_ALPHA8_NV                          0x8C45


#define _EXT_COMPRESSED_SRGB_S3TC_DXT1_NV                   0x8C4C
#define _EXT_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV             0x8C4D
#define _EXT_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV             0x8C4E
#define _EXT_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV             0x8C4F



#define _EXT_ATC_RGB_AMD                        0x8C92
#define _EXT_ATC_RGBA_EXPLICIT_ALPHA_AMD        0x8C93
#define _EXT_ATC_RGBA_INTERPOLATED_ALPHA_AMD    0x87EE


#define _EXT_TEXTURE_CUBE_MAP_SEAMLESS                   0x884F

#define _GL_TEXTURE_MAX_ANISOTROPY_EXT          0x84FE
#define _GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT      0x84FF

#define _EXT_COMPRESSED_R11_EAC 0x9270
#define _EXT_COMPRESSED_SIGNED_R11_EAC 0x9271
#define _EXT_COMPRESSED_RG11_EAC 0x9272
#define _EXT_COMPRESSED_SIGNED_RG11_EAC 0x9273
#define _EXT_COMPRESSED_RGB8_ETC2 0x9274
#define _EXT_COMPRESSED_SRGB8_ETC2 0x9275
#define _EXT_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#define _EXT_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277
#define _EXT_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#define _EXT_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279

#define _EXT_COMPRESSED_RGBA_BPTC_UNORM 0x8E8C
#define _EXT_COMPRESSED_SRGB_ALPHA_BPTC_UNORM 0x8E8D
#define _EXT_COMPRESSED_RGB_BPTC_SIGNED_FLOAT 0x8E8E
#define _EXT_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT 0x8E8F

Image RasterizerStorageGLES3::_get_gl_image_and_format(const Image& p_image, Image::Format p_format, uint32_t p_flags,GLenum& r_gl_format,GLenum& r_gl_internal_format,GLenum &r_gl_type,bool &r_compressed,bool &srgb) {


	r_compressed=false;
	r_gl_format=0;
	Image image=p_image;
	srgb=false;

	bool need_decompress=false;

	switch(p_format) {

		case Image::FORMAT_L8: {
			r_gl_internal_format=GL_R8;
			r_gl_format=GL_RED;
			r_gl_type=GL_UNSIGNED_BYTE;

		} break;
		case Image::FORMAT_LA8: {

			r_gl_internal_format=GL_RG8;
			r_gl_format=GL_RG;
			r_gl_type=GL_UNSIGNED_BYTE;
		} break;
		case Image::FORMAT_R8: {

			r_gl_internal_format=GL_R8;
			r_gl_format=GL_RED;
			r_gl_type=GL_UNSIGNED_BYTE;

		} break;
		case Image::FORMAT_RG8: {

			r_gl_internal_format=GL_RG8;
			r_gl_format=GL_RG;
			r_gl_type=GL_UNSIGNED_BYTE;

		} break;
		case Image::FORMAT_RGB8: {

			r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?GL_SRGB8:GL_RGB8;
			r_gl_format=GL_RGB;
			r_gl_type=GL_UNSIGNED_BYTE;
			srgb=true;

		} break;
		case Image::FORMAT_RGBA8: {

			r_gl_format=GL_RGBA;
			r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?GL_SRGB8_ALPHA8:GL_RGBA8;
			r_gl_type=GL_UNSIGNED_BYTE;
			srgb=true;

		} break;
		case Image::FORMAT_RGB565: {
//#warning TODO: Convert tod 555 if 565 is not supported (GLES3.3-)
			r_gl_internal_format=GL_RGB5;
			//r_gl_internal_format=GL_RGB565;
			r_gl_format=GL_RGB;
			r_gl_type=GL_UNSIGNED_SHORT_5_6_5;

		} break;
		case Image::FORMAT_RGBA4444: {

			r_gl_internal_format=GL_RGBA4;
			r_gl_format=GL_RGBA;
			r_gl_type=GL_UNSIGNED_SHORT_4_4_4_4;

		} break;
		case Image::FORMAT_RGBA5551: {

			r_gl_internal_format=GL_RGB5_A1;
			r_gl_format=GL_RGBA;
			r_gl_type=GL_UNSIGNED_SHORT_5_5_5_1;


		} break;
		case Image::FORMAT_RF: {


			r_gl_internal_format=GL_R32F;
			r_gl_format=GL_RED;
			r_gl_type=GL_FLOAT;

		} break;
		case Image::FORMAT_RGF: {

			r_gl_internal_format=GL_RG32F;
			r_gl_format=GL_RG;
			r_gl_type=GL_FLOAT;

		} break;
		case Image::FORMAT_RGBF: {

			r_gl_internal_format=GL_RGB32F;
			r_gl_format=GL_RGB;
			r_gl_type=GL_FLOAT;

		} break;
		case Image::FORMAT_RGBAF: {

			r_gl_internal_format=GL_RGBA32F;
			r_gl_format=GL_RGBA;
			r_gl_type=GL_FLOAT;

		} break;
		case Image::FORMAT_RH: {
			r_gl_internal_format=GL_R32F;
			r_gl_format=GL_RED;
			r_gl_type=GL_HALF_FLOAT;
		} break;
		case Image::FORMAT_RGH: {
			r_gl_internal_format=GL_RG32F;
			r_gl_format=GL_RG;
			r_gl_type=GL_HALF_FLOAT;

		} break;
		case Image::FORMAT_RGBH: {
			r_gl_internal_format=GL_RGB32F;
			r_gl_format=GL_RGB;
			r_gl_type=GL_HALF_FLOAT;

		} break;
		case Image::FORMAT_RGBAH: {
			r_gl_internal_format=GL_RGBA32F;
			r_gl_format=GL_RGBA;
			r_gl_type=GL_HALF_FLOAT;

		} break;
		case Image::FORMAT_DXT1: {

			if (config.s3tc_supported) {


				r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?_EXT_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV:_EXT_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;

			} else {

				need_decompress=true;
			}


		} break;
		case Image::FORMAT_DXT3: {


			if (config.s3tc_supported) {


				r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?_EXT_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV:_EXT_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;

			} else {

				need_decompress=true;
			}


		} break;
		case Image::FORMAT_DXT5: {

			if (config.s3tc_supported) {


				r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?_EXT_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV:_EXT_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;

			} else {

				need_decompress=true;
			}


		} break;
		case Image::FORMAT_ATI1: {

			if (config.latc_supported) {


				r_gl_internal_format=_EXT_COMPRESSED_LUMINANCE_LATC1_EXT;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;

			} else {

				need_decompress=true;
			}



		} break;
		case Image::FORMAT_ATI2: {

			if (config.latc_supported) {


				r_gl_internal_format=_EXT_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
			} else {

				need_decompress=true;
			}

		} break;
		case Image::FORMAT_BPTC_RGBA: {

			if (config.bptc_supported) {


				r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?_EXT_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:_EXT_COMPRESSED_RGBA_BPTC_UNORM;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;

			} else {

				need_decompress=true;
			}
		} break;
		case Image::FORMAT_BPTC_RGBF: {

			if (config.bptc_supported) {


				r_gl_internal_format=_EXT_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
				r_gl_format=GL_RGB;
				r_gl_type=GL_FLOAT;
				r_compressed=true;
			} else {

				need_decompress=true;
			}
		} break;
		case Image::FORMAT_BPTC_RGBFU: {
			if (config.bptc_supported) {


				r_gl_internal_format=_EXT_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
				r_gl_format=GL_RGB;
				r_gl_type=GL_FLOAT;
				r_compressed=true;
			} else {

				need_decompress=true;
			}
		} break;
		case Image::FORMAT_PVRTC2: {

			if (config.pvrtc_supported) {


				r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?_EXT_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT:_EXT_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;

			} else {

				need_decompress=true;
			}
		} break;
		case Image::FORMAT_PVRTC2A: {

			if (config.pvrtc_supported) {


				r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?_EXT_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT:_EXT_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;

			} else {

				need_decompress=true;
			}


		} break;
		case Image::FORMAT_PVRTC4: {

			if (config.pvrtc_supported) {


				r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?_EXT_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT:_EXT_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;

			} else {

				need_decompress=true;
			}

		} break;
		case Image::FORMAT_PVRTC4A: {

			if (config.pvrtc_supported) {


				r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?_EXT_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT:_EXT_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;

			} else {

				need_decompress=true;
			}


		} break;
		case Image::FORMAT_ETC: {

			if (config.etc_supported) {

				r_gl_internal_format=_EXT_ETC1_RGB8_OES;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;

			} else {

				need_decompress=true;
			}

		} break;
		case Image::FORMAT_ETC2_R11: {

			if (config.etc2_supported) {

				r_gl_internal_format=_EXT_COMPRESSED_R11_EAC;
				r_gl_format=GL_RED;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;

			} else {

				need_decompress=true;
			}
		} break;
		case Image::FORMAT_ETC2_R11S: {

			if (config.etc2_supported) {

				r_gl_internal_format=_EXT_COMPRESSED_SIGNED_R11_EAC;
				r_gl_format=GL_RED;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;

			} else {

				need_decompress=true;
			}
		} break;
		case Image::FORMAT_ETC2_RG11: {

			if (config.etc2_supported) {

				r_gl_internal_format=_EXT_COMPRESSED_RG11_EAC;
				r_gl_format=GL_RG;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;

			} else {

				need_decompress=true;
			}
		} break;
		case Image::FORMAT_ETC2_RG11S: {
			if (config.etc2_supported) {

				r_gl_internal_format=_EXT_COMPRESSED_SIGNED_RG11_EAC;
				r_gl_format=GL_RG;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;

			} else {
				need_decompress=true;
			}
		} break;
		case Image::FORMAT_ETC2_RGB8: {

			if (config.etc2_supported) {

				r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?_EXT_COMPRESSED_SRGB8_ETC2:_EXT_COMPRESSED_RGB8_ETC2;
				r_gl_format=GL_RGB;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;


			} else {

				need_decompress=true;
			}
		} break;
		case Image::FORMAT_ETC2_RGBA8: {

			if (config.etc2_supported) {

				r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?_EXT_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:_EXT_COMPRESSED_RGBA8_ETC2_EAC;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;


			} else {

				need_decompress=true;
			}
		} break;
		case Image::FORMAT_ETC2_RGB8A1: {

			if (config.etc2_supported) {

				r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?_EXT_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:_EXT_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
				r_gl_format=GL_RGBA;
				r_gl_type=GL_UNSIGNED_BYTE;
				r_compressed=true;
				srgb=true;


			} else {

				need_decompress=true;
			}
		} break;
		default: {

			ERR_FAIL_V(Image());
		}
	}

	if (need_decompress) {

		if (!image.empty()) {
			image.decompress();
			ERR_FAIL_COND_V(image.is_compressed(),image);
			image.convert(Image::FORMAT_RGBA8);
		}


		r_gl_format=GL_RGBA;
		r_gl_internal_format=(config.srgb_decode_supported || p_flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)?GL_SRGB8_ALPHA8:GL_RGBA8;
		r_gl_type=GL_UNSIGNED_BYTE;
		r_compressed=false;
		srgb=true;

		return image;

	}


	return image;
}

static const GLenum _cube_side_enum[6]={

	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,

};

RID RasterizerStorageGLES3::texture_create() {

	Texture *texture = memnew(Texture);
	ERR_FAIL_COND_V(!texture,RID());
	glGenTextures(1, &texture->tex_id);
	texture->active=false;
	texture->total_data_size=0;

	return texture_owner.make_rid( texture );

}

void RasterizerStorageGLES3::texture_allocate(RID p_texture,int p_width, int p_height,Image::Format p_format,uint32_t p_flags) {

	int components;
	GLenum format;
	GLenum internal_format;
	GLenum type;

	bool compressed;
	bool srgb;

	if (p_flags&VS::TEXTURE_FLAG_USED_FOR_STREAMING) {
		p_flags&=~VS::TEXTURE_FLAG_MIPMAPS; // no mipies for video
	}


	Texture *texture = texture_owner.get( p_texture );
	ERR_FAIL_COND(!texture);
	texture->width=p_width;
	texture->height=p_height;
	texture->format=p_format;
	texture->flags=p_flags;
	texture->stored_cube_sides=0;
	texture->target = (p_flags & VS::TEXTURE_FLAG_CUBEMAP) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;

	_get_gl_image_and_format(Image(),texture->format,texture->flags,format,internal_format,type,compressed,srgb);

	texture->alloc_width = texture->width;
	texture->alloc_height = texture->height;


	texture->gl_format_cache=format;
	texture->gl_type_cache=type;
	texture->gl_internal_format_cache=internal_format;
	texture->compressed=compressed;
	texture->srgb=srgb;
	texture->data_size=0;
	texture->mipmaps=1;


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texture->target, texture->tex_id);


	if (p_flags&VS::TEXTURE_FLAG_USED_FOR_STREAMING) {
		//prealloc if video
		glTexImage2D(texture->target, 0, internal_format, p_width, p_height, 0, format, type,NULL);
	}

	texture->active=true;
}

void RasterizerStorageGLES3::texture_set_data(RID p_texture,const Image& p_image,VS::CubeMapSide p_cube_side) {

	Texture * texture = texture_owner.get(p_texture);

	ERR_FAIL_COND(!texture);
	ERR_FAIL_COND(!texture->active);
	ERR_FAIL_COND(texture->render_target);
	ERR_FAIL_COND(texture->format != p_image.get_format() );
	ERR_FAIL_COND( p_image.empty() );

	GLenum type;
	GLenum format;
	GLenum internal_format;
	bool compressed;
	bool srgb;


	if (config.keep_original_textures && !(texture->flags&VS::TEXTURE_FLAG_USED_FOR_STREAMING)) {
		texture->images[p_cube_side]=p_image;
	}

	Image img = _get_gl_image_and_format(p_image, p_image.get_format(),texture->flags,format,internal_format,type,compressed,srgb);

	if (config.shrink_textures_x2 && (p_image.has_mipmaps() || !p_image.is_compressed()) && !(texture->flags&VS::TEXTURE_FLAG_USED_FOR_STREAMING)) {

		texture->alloc_height = MAX(1,texture->alloc_height/2);
		texture->alloc_width = MAX(1,texture->alloc_width/2);

		if (texture->alloc_width == img.get_width()/2 && texture->alloc_height == img.get_height()/2) {

			img.shrink_x2();
		} else if (img.get_format() <= Image::FORMAT_RGB565) {

			img.resize(texture->alloc_width, texture->alloc_height, Image::INTERPOLATE_BILINEAR);

		}
	};


	GLenum blit_target = (texture->target == GL_TEXTURE_CUBE_MAP)?_cube_side_enum[p_cube_side]:GL_TEXTURE_2D;

	texture->data_size=img.get_data().size();
	PoolVector<uint8_t>::Read read = img.get_data().read();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texture->target, texture->tex_id);

	texture->ignore_mipmaps = compressed && !img.has_mipmaps();

	if (texture->flags&VS::TEXTURE_FLAG_MIPMAPS && !texture->ignore_mipmaps)
		glTexParameteri(texture->target,GL_TEXTURE_MIN_FILTER,config.use_fast_texture_filter?GL_LINEAR_MIPMAP_NEAREST:GL_LINEAR_MIPMAP_LINEAR);
	else {
		if (texture->flags&VS::TEXTURE_FLAG_FILTER) {
			glTexParameteri(texture->target,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		} else {
			glTexParameteri(texture->target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

		}
	}


	if (config.srgb_decode_supported && srgb) {

		if (texture->flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR) {

			glTexParameteri(texture->target,_TEXTURE_SRGB_DECODE_EXT,_DECODE_EXT);
			texture->using_srgb=true;
		} else {
			glTexParameteri(texture->target,_TEXTURE_SRGB_DECODE_EXT,_SKIP_DECODE_EXT);
			texture->using_srgb=false;
		}
	}

	if (texture->flags&VS::TEXTURE_FLAG_FILTER) {

		glTexParameteri(texture->target,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering

	} else {

		glTexParameteri(texture->target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	// raw Filtering
	}

	if ((texture->flags&VS::TEXTURE_FLAG_REPEAT || texture->flags&VS::TEXTURE_FLAG_MIRRORED_REPEAT) && texture->target != GL_TEXTURE_CUBE_MAP) {

		if (texture->flags&VS::TEXTURE_FLAG_MIRRORED_REPEAT){
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
		}
		else{
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		}
	} else {

		//glTexParameterf( texture->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
		glTexParameterf( texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	}

	//set swizle for older format compatibility
	switch(texture->format) {

		case Image::FORMAT_L8: {
			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_R,GL_RED);
			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_G,GL_RED);
			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_B,GL_RED);
			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_A,GL_ONE);

		} break;
		case Image::FORMAT_LA8: {

			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_R,GL_RED);
			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_G,GL_RED);
			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_B,GL_RED);
			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_A,GL_GREEN);
		} break;
		default: {
			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_R,GL_RED);
			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_G,GL_GREEN);
			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_B,GL_BLUE);
			glTexParameteri(texture->target,GL_TEXTURE_SWIZZLE_A,GL_ALPHA);

		} break;

	}
	if (config.use_anisotropic_filter) {

		if (texture->flags&VS::TEXTURE_FLAG_ANISOTROPIC_FILTER) {

			glTexParameterf(texture->target, _GL_TEXTURE_MAX_ANISOTROPY_EXT, config.anisotropic_level);
		} else {
			glTexParameterf(texture->target, _GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
		}
	}

	int mipmaps= (texture->flags&VS::TEXTURE_FLAG_MIPMAPS && img.has_mipmaps()) ? img.get_mipmap_count() +1: 1;


	int w=img.get_width();
	int h=img.get_height();

	int tsize=0;
	for(int i=0;i<mipmaps;i++) {

		int size,ofs;
		img.get_mipmap_offset_and_size(i,ofs,size);

		//print_line("mipmap: "+itos(i)+" size: "+itos(size)+" w: "+itos(mm_w)+", h: "+itos(mm_h));

		if (texture->compressed) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glCompressedTexImage2D( blit_target, i, format,w,h,0,size,&read[ofs] );

		} else {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			if (texture->flags&VS::TEXTURE_FLAG_USED_FOR_STREAMING) {
				glTexSubImage2D( blit_target, i, 0,0,w, h,format,type,&read[ofs] );
			} else {
				glTexImage2D(blit_target, i, internal_format, w, h, 0, format, type,&read[ofs]);
			}

		}
		tsize+=size;

		w = MAX(1,w>>1);
		h = MAX(1,h>>1);

	}

	info.texture_mem-=texture->total_data_size;
	texture->total_data_size=tsize;
	info.texture_mem+=texture->total_data_size;

	//printf("texture: %i x %i - size: %i - total: %i\n",texture->width,texture->height,tsize,_rinfo.texture_mem);

	texture->stored_cube_sides|=(1<<p_cube_side);

	if (texture->flags&VS::TEXTURE_FLAG_MIPMAPS && mipmaps==1 && !texture->ignore_mipmaps && (!(texture->flags&VS::TEXTURE_FLAG_CUBEMAP) || texture->stored_cube_sides==(1<<6)-1)) {
		//generate mipmaps if they were requested and the image does not contain them
		glGenerateMipmap(texture->target);
	}

	texture->mipmaps=mipmaps;

	//texture_set_flags(p_texture,texture->flags);


}

Image RasterizerStorageGLES3::texture_get_data(RID p_texture,VS::CubeMapSide p_cube_side) const {

	Texture * texture = texture_owner.get(p_texture);

	ERR_FAIL_COND_V(!texture,Image());
	ERR_FAIL_COND_V(!texture->active,Image());
	ERR_FAIL_COND_V(texture->data_size==0,Image());
	ERR_FAIL_COND_V(texture->render_target,Image());

	if (!texture->images[p_cube_side].empty())
		return texture->images[p_cube_side];

#ifdef GLES_OVER_GL

	PoolVector<uint8_t> data;

	int data_size = Image::get_image_data_size(texture->alloc_width,texture->alloc_height,texture->format,texture->mipmaps>1?-1:0);

	data.resize(data_size*2); //add some memory at the end, just in case for buggy drivers
	PoolVector<uint8_t>::Write wb = data.write();

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(texture->target,texture->tex_id);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	print_line("GET FORMAT: "+Image::get_format_name(texture->format)+" mipmaps: "+itos(texture->mipmaps));


	for(int i=0;i<texture->mipmaps;i++) {

		int ofs=0;
		if (i>0) {
			ofs=Image::get_image_data_size(texture->alloc_width,texture->alloc_height,texture->format,i-1);
		}

		if (texture->compressed) {

			glPixelStorei(GL_PACK_ALIGNMENT, 4);
			glGetCompressedTexImage(texture->target,i,&wb[ofs]);

		} else {

			glPixelStorei(GL_PACK_ALIGNMENT, 1);

			glGetTexImage(texture->target,i,texture->gl_format_cache,texture->gl_type_cache,&wb[ofs]);
		}
	}


	wb=PoolVector<uint8_t>::Write();

	data.resize(data_size);

	Image img(texture->alloc_width,texture->alloc_height,texture->mipmaps>1?true:false,texture->format,data);

	return img;
#else

	ERR_EXPLAIN("Sorry, It's not posible to obtain images back in OpenGL ES");
	return Image();
#endif
}

void RasterizerStorageGLES3::texture_set_flags(RID p_texture,uint32_t p_flags) {

	Texture *texture = texture_owner.get( p_texture );
	ERR_FAIL_COND(!texture);
	if (texture->render_target) {

		p_flags&=VS::TEXTURE_FLAG_FILTER;//can change only filter
	}

	bool had_mipmaps = texture->flags&VS::TEXTURE_FLAG_MIPMAPS;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texture->target, texture->tex_id);
	uint32_t cube = texture->flags & VS::TEXTURE_FLAG_CUBEMAP;
	texture->flags=p_flags|cube; // can't remove a cube from being a cube


	if ((texture->flags&VS::TEXTURE_FLAG_REPEAT || texture->flags&VS::TEXTURE_FLAG_MIRRORED_REPEAT) && texture->target != GL_TEXTURE_CUBE_MAP) {

		if (texture->flags&VS::TEXTURE_FLAG_MIRRORED_REPEAT){
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
		}
		else {
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		}
	} else {
		//glTexParameterf( texture->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
		glTexParameterf( texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	}


	if (config.use_anisotropic_filter) {

		if (texture->flags&VS::TEXTURE_FLAG_ANISOTROPIC_FILTER) {

			glTexParameterf(texture->target, _GL_TEXTURE_MAX_ANISOTROPY_EXT, config.anisotropic_level);
		} else {
			glTexParameterf(texture->target, _GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
		}
	}

	if (texture->flags&VS::TEXTURE_FLAG_MIPMAPS && !texture->ignore_mipmaps) {
		if (!had_mipmaps && texture->mipmaps==1) {
			glGenerateMipmap(texture->target);
		}
		glTexParameteri(texture->target,GL_TEXTURE_MIN_FILTER,config.use_fast_texture_filter?GL_LINEAR_MIPMAP_NEAREST:GL_LINEAR_MIPMAP_LINEAR);

	} else{
		if (texture->flags&VS::TEXTURE_FLAG_FILTER) {
			glTexParameteri(texture->target,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		} else {
			glTexParameteri(texture->target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

		}
	}


	if (config.srgb_decode_supported && texture->srgb) {

		if (texture->flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR) {

			glTexParameteri(texture->target,_TEXTURE_SRGB_DECODE_EXT,_DECODE_EXT);
			texture->using_srgb=true;
		} else {
			glTexParameteri(texture->target,_TEXTURE_SRGB_DECODE_EXT,_SKIP_DECODE_EXT);
			texture->using_srgb=false;
		}
	}

	if (texture->flags&VS::TEXTURE_FLAG_FILTER) {

		glTexParameteri(texture->target,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering

	} else {

		glTexParameteri(texture->target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	// raw Filtering
	}

}
uint32_t RasterizerStorageGLES3::texture_get_flags(RID p_texture) const {

	Texture * texture = texture_owner.get(p_texture);

	ERR_FAIL_COND_V(!texture,0);

	return texture->flags;

}
Image::Format RasterizerStorageGLES3::texture_get_format(RID p_texture) const {

	Texture * texture = texture_owner.get(p_texture);

	ERR_FAIL_COND_V(!texture,Image::FORMAT_L8);

	return texture->format;
}
uint32_t RasterizerStorageGLES3::texture_get_width(RID p_texture) const {

	Texture * texture = texture_owner.get(p_texture);

	ERR_FAIL_COND_V(!texture,0);

	return texture->width;
}
uint32_t RasterizerStorageGLES3::texture_get_height(RID p_texture) const {

	Texture * texture = texture_owner.get(p_texture);

	ERR_FAIL_COND_V(!texture,0);

	return texture->height;
}


void RasterizerStorageGLES3::texture_set_size_override(RID p_texture,int p_width, int p_height) {

	Texture * texture = texture_owner.get(p_texture);

	ERR_FAIL_COND(!texture);
	ERR_FAIL_COND(texture->render_target);

	ERR_FAIL_COND(p_width<=0 || p_width>16384);
	ERR_FAIL_COND(p_height<=0 || p_height>16384);
	//real texture size is in alloc width and height
	texture->width=p_width;
	texture->height=p_height;

}

void RasterizerStorageGLES3::texture_set_path(RID p_texture,const String& p_path) {
	Texture * texture = texture_owner.get(p_texture);
	ERR_FAIL_COND(!texture);

	texture->path=p_path;

}

String RasterizerStorageGLES3::texture_get_path(RID p_texture) const{

	Texture * texture = texture_owner.get(p_texture);
	ERR_FAIL_COND_V(!texture,String());
	return texture->path;
}
void RasterizerStorageGLES3::texture_debug_usage(List<VS::TextureInfo> *r_info){

	List<RID> textures;
	texture_owner.get_owned_list(&textures);

	for (List<RID>::Element *E=textures.front();E;E=E->next()) {

		Texture *t = texture_owner.get(E->get());
		if (!t)
			continue;
		VS::TextureInfo tinfo;
		tinfo.path=t->path;
		tinfo.format=t->format;
		tinfo.size.x=t->alloc_width;
		tinfo.size.y=t->alloc_height;
		tinfo.bytes=t->total_data_size;
		r_info->push_back(tinfo);
	}

}

void RasterizerStorageGLES3::texture_set_shrink_all_x2_on_set_data(bool p_enable) {

	config.shrink_textures_x2=p_enable;
}

void RasterizerStorageGLES3::textures_keep_original(bool p_enable) {

	config.keep_original_textures=p_enable;
}

RID RasterizerStorageGLES3::texture_create_radiance_cubemap(RID p_source,int p_resolution) const {

	Texture * texture = texture_owner.get(p_source);
	ERR_FAIL_COND_V(!texture,RID());
	ERR_FAIL_COND_V(!(texture->flags&VS::TEXTURE_FLAG_CUBEMAP),RID());

	bool use_float=true;

	if (p_resolution<0) {
		p_resolution=texture->width;
	}


	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_BLEND);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texture->target, texture->tex_id);

	if (config.srgb_decode_supported && texture->srgb && !texture->using_srgb) {

		glTexParameteri(texture->target,_TEXTURE_SRGB_DECODE_EXT,_DECODE_EXT);
		texture->using_srgb=true;
#ifdef TOOLS_ENABLED
		if (!(texture->flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)) {
			texture->flags|=VS::TEXTURE_FLAG_CONVERT_TO_LINEAR;
			//notify that texture must be set to linear beforehand, so it works in other platforms when exported
		}
#endif
	}


	glActiveTexture(GL_TEXTURE1);
	GLuint new_cubemap;
	glGenTextures(1, &new_cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, new_cubemap);


	GLuint tmp_fb;

	glGenFramebuffers(1, &tmp_fb);
	glBindFramebuffer(GL_FRAMEBUFFER, tmp_fb);


	int size = p_resolution;

	int lod=0;

	shaders.cubemap_filter.bind();

	int mipmaps=6;

	int mm_level=mipmaps;

	GLenum internal_format = use_float?GL_RGBA16F:GL_RGB10_A2;
	GLenum format = GL_RGBA;
	GLenum type = use_float?GL_HALF_FLOAT:GL_UNSIGNED_INT_2_10_10_10_REV;


	while(mm_level) {

		for(int i=0;i<6;i++) {
			glTexImage2D(_cube_side_enum[i], lod, internal_format,  size, size, 0, format, type, NULL);
		}

		lod++;
		mm_level--;

		if (size>1)
			size>>=1;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, lod-1);

	lod=0;
	mm_level=mipmaps;

	size = p_resolution;

	shaders.cubemap_filter.set_conditional(CubemapFilterShaderGLES3::USE_DUAL_PARABOLOID,false);

	while(mm_level) {

		for(int i=0;i<6;i++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _cube_side_enum[i], new_cubemap, lod);

			glViewport(0,0,size,size);
			glBindVertexArray(resources.quadie_array);

			shaders.cubemap_filter.set_uniform(CubemapFilterShaderGLES3::FACE_ID,i);
			shaders.cubemap_filter.set_uniform(CubemapFilterShaderGLES3::ROUGHNESS,lod/float(mipmaps-1));


			glDrawArrays(GL_TRIANGLE_FAN,0,4);
			glBindVertexArray(0);
#ifdef DEBUG_ENABLED
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			ERR_CONTINUE(status!=GL_FRAMEBUFFER_COMPLETE);
#endif
		}



		if (size>1)
			size>>=1;
		lod++;
		mm_level--;

	}


	//restore ranges
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, lod-1);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, config.system_fbo);
	glDeleteFramebuffers(1, &tmp_fb);

	Texture * ctex = memnew( Texture );

	ctex->flags=VS::TEXTURE_FLAG_CUBEMAP|VS::TEXTURE_FLAG_MIPMAPS|VS::TEXTURE_FLAG_FILTER;
	ctex->width=p_resolution;
	ctex->height=p_resolution;
	ctex->alloc_width=p_resolution;
	ctex->alloc_height=p_resolution;
	ctex->format=use_float?Image::FORMAT_RGBAH:Image::FORMAT_RGBA8;
	ctex->target=GL_TEXTURE_CUBE_MAP;
	ctex->gl_format_cache=format;
	ctex->gl_internal_format_cache=internal_format;
	ctex->gl_type_cache=type;
	ctex->data_size=0;
	ctex->compressed=false;
	ctex->srgb=false;
	ctex->total_data_size=0;
	ctex->ignore_mipmaps=false;
	ctex->mipmaps=mipmaps;
	ctex->active=true;
	ctex->tex_id=new_cubemap;
	ctex->stored_cube_sides=(1<<6)-1;
	ctex->render_target=NULL;

	return texture_owner.make_rid(ctex);
}


RID RasterizerStorageGLES3::skybox_create() {

	SkyBox *skybox = memnew( SkyBox );
	skybox->radiance=0;
	return skybox_owner.make_rid(skybox);
}

void RasterizerStorageGLES3::skybox_set_texture(RID p_skybox, RID p_cube_map, int p_radiance_size){

	SkyBox *skybox = skybox_owner.getornull(p_skybox);
	ERR_FAIL_COND(!skybox);

	if (skybox->cubemap.is_valid()) {
		skybox->cubemap=RID();
		glDeleteTextures(1,&skybox->radiance);
		skybox->radiance=0;
	}

	skybox->cubemap=p_cube_map;
	if (!skybox->cubemap.is_valid())
		return; //cleared

	Texture *texture = texture_owner.getornull(skybox->cubemap);
	if (!texture || !(texture->flags&VS::TEXTURE_FLAG_CUBEMAP)) {
		skybox->cubemap=RID();
		ERR_FAIL_COND(!texture || !(texture->flags&VS::TEXTURE_FLAG_CUBEMAP));
	}

	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_BLEND);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texture->target, texture->tex_id);

	if (config.srgb_decode_supported && texture->srgb && !texture->using_srgb) {

		glTexParameteri(texture->target,_TEXTURE_SRGB_DECODE_EXT,_DECODE_EXT);
		texture->using_srgb=true;
#ifdef TOOLS_ENABLED
		if (!(texture->flags&VS::TEXTURE_FLAG_CONVERT_TO_LINEAR)) {
			texture->flags|=VS::TEXTURE_FLAG_CONVERT_TO_LINEAR;
			//notify that texture must be set to linear beforehand, so it works in other platforms when exported
		}
#endif
	}


	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &skybox->radiance);
	glBindTexture(GL_TEXTURE_2D, skybox->radiance);

	GLuint tmp_fb;

	glGenFramebuffers(1, &tmp_fb);
	glBindFramebuffer(GL_FRAMEBUFFER, tmp_fb);


	int size = p_radiance_size;

	int lod=0;


	int mipmaps=6;

	int mm_level=mipmaps;

	bool use_float=true;

	GLenum internal_format = use_float?GL_RGBA16F:GL_RGB10_A2;
	GLenum format = GL_RGBA;
	GLenum type = use_float?GL_HALF_FLOAT:GL_UNSIGNED_INT_2_10_10_10_REV;

	while(mm_level) {

		glTexImage2D(GL_TEXTURE_2D, lod, internal_format,  size, size*2, 0, format, type, NULL);
		lod++;
		mm_level--;

		if (size>1)
			size>>=1;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, lod-1);

	lod=0;
	mm_level=mipmaps;

	size = p_radiance_size;

	shaders.cubemap_filter.set_conditional(CubemapFilterShaderGLES3::USE_DUAL_PARABOLOID,true);
	shaders.cubemap_filter.bind();

	while(mm_level) {

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  skybox->radiance, lod);
#ifdef DEBUG_ENABLED
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		ERR_CONTINUE(status!=GL_FRAMEBUFFER_COMPLETE);
#endif

		for(int i=0;i<2;i++) {
			glViewport(0,i*size,size,size);
			glBindVertexArray(resources.quadie_array);

			shaders.cubemap_filter.set_uniform(CubemapFilterShaderGLES3::Z_FLIP,i>0);
			shaders.cubemap_filter.set_uniform(CubemapFilterShaderGLES3::ROUGHNESS,lod/float(mipmaps-1));


			glDrawArrays(GL_TRIANGLE_FAN,0,4);
			glBindVertexArray(0);
		}

		if (size>1)
			size>>=1;
		lod++;
		mm_level--;

	}
	shaders.cubemap_filter.set_conditional(CubemapFilterShaderGLES3::USE_DUAL_PARABOLOID,false);


	//restore ranges
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, lod-1);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, config.system_fbo);
	glDeleteFramebuffers(1, &tmp_fb);

}


/* SHADER API */


RID RasterizerStorageGLES3::shader_create(VS::ShaderMode p_mode){

	Shader *shader = memnew( Shader );
	shader->mode=p_mode;
	RID rid = shader_owner.make_rid(shader);
	shader_set_mode(rid,p_mode);
	_shader_make_dirty(shader);
	shader->self=rid;

	return rid;
}

void RasterizerStorageGLES3::_shader_make_dirty(Shader* p_shader) {

	if (p_shader->dirty_list.in_list())
		return;

	_shader_dirty_list.add(&p_shader->dirty_list);
}

void RasterizerStorageGLES3::shader_set_mode(RID p_shader,VS::ShaderMode p_mode){

	ERR_FAIL_INDEX(p_mode,VS::SHADER_MAX);
	Shader *shader=shader_owner.get(p_shader);
	ERR_FAIL_COND(!shader);

	if (shader->custom_code_id && p_mode==shader->mode)
		return;


	if (shader->custom_code_id) {

		shader->shader->free_custom_shader(shader->custom_code_id);
		shader->custom_code_id=0;
	}

	shader->mode=p_mode;

	ShaderGLES3* shaders[VS::SHADER_MAX]={
		&scene->state.scene_shader,
		&canvas->state.canvas_shader,
		&this->shaders.particles,

	};

	shader->shader=shaders[p_mode];

	shader->custom_code_id = shader->shader->create_custom_shader();

	_shader_make_dirty(shader);

}
VS::ShaderMode RasterizerStorageGLES3::shader_get_mode(RID p_shader) const {

	const Shader *shader=shader_owner.get(p_shader);
	ERR_FAIL_COND_V(!shader,VS::SHADER_MAX);

	return shader->mode;
}
void RasterizerStorageGLES3::shader_set_code(RID p_shader, const String& p_code){

	Shader *shader=shader_owner.get(p_shader);
	ERR_FAIL_COND(!shader);

	shader->code=p_code;
	_shader_make_dirty(shader);
}
String RasterizerStorageGLES3::shader_get_code(RID p_shader) const{

	const Shader *shader=shader_owner.get(p_shader);
	ERR_FAIL_COND_V(!shader,String());


	return shader->code;
}

void RasterizerStorageGLES3::_update_shader(Shader* p_shader) const {


	_shader_dirty_list.remove( &p_shader->dirty_list );

	p_shader->valid=false;

	p_shader->uniforms.clear();

	ShaderCompilerGLES3::GeneratedCode gen_code;
	ShaderCompilerGLES3::IdentifierActions *actions=NULL;



	switch(p_shader->mode) {
		case VS::SHADER_CANVAS_ITEM: {

			p_shader->canvas_item.light_mode=Shader::CanvasItem::LIGHT_MODE_NORMAL;
			p_shader->canvas_item.blend_mode=Shader::CanvasItem::BLEND_MODE_MIX;

			shaders.actions_canvas.render_mode_values["blend_add"]=Pair<int*,int>(&p_shader->canvas_item.blend_mode,Shader::CanvasItem::BLEND_MODE_ADD);
			shaders.actions_canvas.render_mode_values["blend_mix"]=Pair<int*,int>(&p_shader->canvas_item.blend_mode,Shader::CanvasItem::BLEND_MODE_MIX);
			shaders.actions_canvas.render_mode_values["blend_sub"]=Pair<int*,int>(&p_shader->canvas_item.blend_mode,Shader::CanvasItem::BLEND_MODE_SUB);
			shaders.actions_canvas.render_mode_values["blend_mul"]=Pair<int*,int>(&p_shader->canvas_item.blend_mode,Shader::CanvasItem::BLEND_MODE_MUL);
			shaders.actions_canvas.render_mode_values["blend_premul_alpha"]=Pair<int*,int>(&p_shader->canvas_item.blend_mode,Shader::CanvasItem::BLEND_MODE_PMALPHA);

			shaders.actions_canvas.render_mode_values["unshaded"]=Pair<int*,int>(&p_shader->canvas_item.light_mode,Shader::CanvasItem::LIGHT_MODE_UNSHADED);
			shaders.actions_canvas.render_mode_values["light_only"]=Pair<int*,int>(&p_shader->canvas_item.light_mode,Shader::CanvasItem::LIGHT_MODE_LIGHT_ONLY);

			actions=&shaders.actions_canvas;
			actions->uniforms=&p_shader->uniforms;

		} break;

		case VS::SHADER_SPATIAL: {

			p_shader->spatial.blend_mode=Shader::Spatial::BLEND_MODE_MIX;
			p_shader->spatial.depth_draw_mode=Shader::Spatial::DEPTH_DRAW_OPAQUE;
			p_shader->spatial.cull_mode=Shader::Spatial::CULL_MODE_BACK;
			p_shader->spatial.uses_alpha=false;
			p_shader->spatial.uses_discard=false;
			p_shader->spatial.unshaded=false;
			p_shader->spatial.ontop=false;
			p_shader->spatial.uses_sss=false;
			p_shader->spatial.uses_vertex=false;

			shaders.actions_scene.render_mode_values["blend_add"]=Pair<int*,int>(&p_shader->spatial.blend_mode,Shader::Spatial::BLEND_MODE_ADD);
			shaders.actions_scene.render_mode_values["blend_mix"]=Pair<int*,int>(&p_shader->spatial.blend_mode,Shader::Spatial::BLEND_MODE_MIX);
			shaders.actions_scene.render_mode_values["blend_sub"]=Pair<int*,int>(&p_shader->spatial.blend_mode,Shader::Spatial::BLEND_MODE_SUB);
			shaders.actions_scene.render_mode_values["blend_mul"]=Pair<int*,int>(&p_shader->spatial.blend_mode,Shader::Spatial::BLEND_MODE_MUL);

			shaders.actions_scene.render_mode_values["depth_draw_opaque"]=Pair<int*,int>(&p_shader->spatial.depth_draw_mode,Shader::Spatial::DEPTH_DRAW_OPAQUE);
			shaders.actions_scene.render_mode_values["depth_draw_always"]=Pair<int*,int>(&p_shader->spatial.depth_draw_mode,Shader::Spatial::DEPTH_DRAW_ALWAYS);
			shaders.actions_scene.render_mode_values["depth_draw_never"]=Pair<int*,int>(&p_shader->spatial.depth_draw_mode,Shader::Spatial::DEPTH_DRAW_NEVER);
			shaders.actions_scene.render_mode_values["depth_draw_alpha_prepass"]=Pair<int*,int>(&p_shader->spatial.depth_draw_mode,Shader::Spatial::DEPTH_DRAW_ALPHA_PREPASS);

			shaders.actions_scene.render_mode_values["cull_front"]=Pair<int*,int>(&p_shader->spatial.cull_mode,Shader::Spatial::CULL_MODE_FRONT);
			shaders.actions_scene.render_mode_values["cull_back"]=Pair<int*,int>(&p_shader->spatial.cull_mode,Shader::Spatial::CULL_MODE_BACK);
			shaders.actions_scene.render_mode_values["cull_disabled"]=Pair<int*,int>(&p_shader->spatial.cull_mode,Shader::Spatial::CULL_MODE_DISABLED);

			shaders.actions_scene.render_mode_flags["unshaded"]=&p_shader->spatial.unshaded;
			shaders.actions_scene.render_mode_flags["ontop"]=&p_shader->spatial.ontop;

			shaders.actions_scene.usage_flag_pointers["ALPHA"]=&p_shader->spatial.uses_alpha;
			shaders.actions_scene.usage_flag_pointers["VERTEX"]=&p_shader->spatial.uses_vertex;

			shaders.actions_scene.usage_flag_pointers["SSS_STRENGTH"]=&p_shader->spatial.uses_sss;
			shaders.actions_scene.usage_flag_pointers["DISCARD"]=&p_shader->spatial.uses_discard;

			actions=&shaders.actions_scene;
			actions->uniforms=&p_shader->uniforms;


		}
		case VS::SHADER_PARTICLES: {

			actions=&shaders.actions_particles;
			actions->uniforms=&p_shader->uniforms;
		}

	}


	Error err = shaders.compiler.compile(p_shader->mode,p_shader->code,actions,p_shader->path,gen_code);


	ERR_FAIL_COND(err!=OK);

	p_shader->shader->set_custom_shader_code(p_shader->custom_code_id,gen_code.vertex,gen_code.vertex_global,gen_code.fragment,gen_code.light,gen_code.fragment_global,gen_code.uniforms,gen_code.texture_uniforms,gen_code.defines);

	p_shader->ubo_size=gen_code.uniform_total_size;
	p_shader->ubo_offsets=gen_code.uniform_offsets;
	p_shader->texture_count=gen_code.texture_uniforms.size();
	p_shader->texture_hints=gen_code.texture_hints;

	p_shader->uses_vertex_time=gen_code.uses_vertex_time;
	p_shader->uses_fragment_time=gen_code.uses_fragment_time;

	//all materials using this shader will have to be invalidated, unfortunately

	for (SelfList<Material>* E = p_shader->materials.first();E;E=E->next() ) {

		_material_make_dirty(E->self());
	}

	p_shader->valid=true;
	p_shader->version++;


}

void RasterizerStorageGLES3::update_dirty_shaders() {

	while( _shader_dirty_list.first() ) {
		_update_shader(_shader_dirty_list.first()->self() );
	}
}

void RasterizerStorageGLES3::shader_get_param_list(RID p_shader, List<PropertyInfo> *p_param_list) const{

	Shader *shader=shader_owner.get(p_shader);
	ERR_FAIL_COND(!shader);


	if (shader->dirty_list.in_list())
		_update_shader(shader); // ok should be not anymore dirty


	Map<int,StringName> order;


	for(Map<StringName,ShaderLanguage::ShaderNode::Uniform>::Element *E=shader->uniforms.front();E;E=E->next()) {


		order[E->get().order]=E->key();
	}


	for(Map<int,StringName>::Element *E=order.front();E;E=E->next()) {

		PropertyInfo pi;
		ShaderLanguage::ShaderNode::Uniform &u=shader->uniforms[E->get()];
		pi.name=E->get();
		switch(u.type) {
			case ShaderLanguage::TYPE_VOID: pi.type=Variant::NIL; break;
			case ShaderLanguage::TYPE_BOOL: pi.type=Variant::BOOL; break;
			case ShaderLanguage::TYPE_BVEC2: pi.type=Variant::INT; pi.hint=PROPERTY_HINT_FLAGS; pi.hint_string="x,y"; break;
			case ShaderLanguage::TYPE_BVEC3: pi.type=Variant::INT; pi.hint=PROPERTY_HINT_FLAGS; pi.hint_string="x,y,z"; break;
			case ShaderLanguage::TYPE_BVEC4: pi.type=Variant::INT; pi.hint=PROPERTY_HINT_FLAGS; pi.hint_string="x,y,z,w"; break;
			case ShaderLanguage::TYPE_UINT:
			case ShaderLanguage::TYPE_INT: {
				pi.type=Variant::INT;
				if (u.hint==ShaderLanguage::ShaderNode::Uniform::HINT_RANGE) {
					pi.hint=PROPERTY_HINT_RANGE;
					pi.hint_string=rtos(u.hint_range[0])+","+rtos(u.hint_range[1]);
				}

			} break;
			case ShaderLanguage::TYPE_IVEC2:
			case ShaderLanguage::TYPE_IVEC3:
			case ShaderLanguage::TYPE_IVEC4:
			case ShaderLanguage::TYPE_UVEC2:
			case ShaderLanguage::TYPE_UVEC3:
			case ShaderLanguage::TYPE_UVEC4: {

				pi.type=Variant::POOL_INT_ARRAY;
			} break;
			case ShaderLanguage::TYPE_FLOAT: {
				pi.type=Variant::REAL;
				if (u.hint==ShaderLanguage::ShaderNode::Uniform::HINT_RANGE) {
					pi.hint=PROPERTY_HINT_RANGE;
					pi.hint_string=rtos(u.hint_range[0])+","+rtos(u.hint_range[1])+","+rtos(u.hint_range[2]);
				}

			} break;
			case ShaderLanguage::TYPE_VEC2: pi.type=Variant::VECTOR2; break;
			case ShaderLanguage::TYPE_VEC3: pi.type=Variant::VECTOR3; break;
			case ShaderLanguage::TYPE_VEC4: {
				if (u.hint==ShaderLanguage::ShaderNode::Uniform::HINT_COLOR) {
					pi.type=Variant::COLOR;
				} else {
					pi.type=Variant::PLANE;
				}
			} break;
			case ShaderLanguage::TYPE_MAT2: pi.type=Variant::TRANSFORM2D; break;
			case ShaderLanguage::TYPE_MAT3: pi.type=Variant::BASIS; break;
			case ShaderLanguage::TYPE_MAT4: pi.type=Variant::TRANSFORM; break;
			case ShaderLanguage::TYPE_SAMPLER2D:
			case ShaderLanguage::TYPE_ISAMPLER2D:
			case ShaderLanguage::TYPE_USAMPLER2D: {

				 pi.type=Variant::OBJECT;
				 pi.hint=PROPERTY_HINT_RESOURCE_TYPE;
				 pi.hint_string="Texture";
			} break;
			case ShaderLanguage::TYPE_SAMPLERCUBE: {

				pi.type=Variant::OBJECT;
				pi.hint=PROPERTY_HINT_RESOURCE_TYPE;
				pi.hint_string="CubeMap";
			} break;
		};

		p_param_list->push_back(pi);

	}
}

void RasterizerStorageGLES3::shader_set_default_texture_param(RID p_shader, const StringName& p_name, RID p_texture){

	Shader *shader=shader_owner.get(p_shader);
	ERR_FAIL_COND(!shader);
	ERR_FAIL_COND(p_texture.is_valid() && !texture_owner.owns(p_texture));

	if (p_texture.is_valid())
		shader->default_textures[p_name]=p_texture;
	else
		shader->default_textures.erase(p_name);

	_shader_make_dirty(shader);
}
RID RasterizerStorageGLES3::shader_get_default_texture_param(RID p_shader, const StringName& p_name) const{

	const Shader *shader=shader_owner.get(p_shader);
	ERR_FAIL_COND_V(!shader,RID());

	const Map<StringName,RID>::Element *E=shader->default_textures.find(p_name);
	if (!E)
		return RID();
	return E->get();
}


/* COMMON MATERIAL API */

void RasterizerStorageGLES3::_material_make_dirty(Material* p_material) const {

	if (p_material->dirty_list.in_list())
		return;

	_material_dirty_list.add(&p_material->dirty_list);
}

RID RasterizerStorageGLES3::material_create(){

	Material *material = memnew( Material );

	return material_owner.make_rid(material);
}

void RasterizerStorageGLES3::material_set_shader(RID p_material, RID p_shader){

	Material *material = material_owner.get(  p_material );
	ERR_FAIL_COND(!material);

	Shader *shader=shader_owner.getornull(p_shader);

	if (material->shader) {
		//if shader, remove from previous shader material list
		material->shader->materials.remove( &material->list );
	}
	material->shader=shader;

	if (shader) {
		shader->materials.add(&material->list);
	}

	_material_make_dirty(material);

}

RID RasterizerStorageGLES3::material_get_shader(RID p_material) const{

	const Material *material = material_owner.get(  p_material );
	ERR_FAIL_COND_V(!material,RID());

	if (material->shader)
		return material->shader->self;

	return RID();
}

void RasterizerStorageGLES3::material_set_param(RID p_material, const StringName& p_param, const Variant& p_value){

	Material *material = material_owner.get(  p_material );
	ERR_FAIL_COND(!material);

	if (p_value.get_type()==Variant::NIL)
		material->params.erase(p_param);
	else
		material->params[p_param]=p_value;

	_material_make_dirty(material);

}
Variant RasterizerStorageGLES3::material_get_param(RID p_material, const StringName& p_param) const{

	const Material *material = material_owner.get(  p_material );
	ERR_FAIL_COND_V(!material,RID());

	if (material->params.has(p_param))
		return material->params[p_param];

	return Variant();
}

void RasterizerStorageGLES3::material_set_line_width(RID p_material, float p_width) {

	Material *material = material_owner.get(  p_material );
	ERR_FAIL_COND(!material);

	material->line_width=p_width;


}

bool RasterizerStorageGLES3::material_is_animated(RID p_material)  {

	Material *material = material_owner.get(  p_material );
	ERR_FAIL_COND_V(!material,false);
	if (material->dirty_list.in_list()) {
		_update_material(material);
	}

	return material->is_animated_cache;

}
bool RasterizerStorageGLES3::material_casts_shadows(RID p_material)  {

	Material *material = material_owner.get(  p_material );
	ERR_FAIL_COND_V(!material,false);
	if (material->dirty_list.in_list()) {
		_update_material(material);
	}

	return material->can_cast_shadow_cache;
}

void RasterizerStorageGLES3::material_add_instance_owner(RID p_material, RasterizerScene::InstanceBase *p_instance) {

	Material *material = material_owner.get(  p_material );
	ERR_FAIL_COND(!material);

	Map<RasterizerScene::InstanceBase*,int>::Element *E=material->instance_owners.find(p_instance);
	if (E) {
		E->get()++;
	} else {
		material->instance_owners[p_instance]=1;
	}
}

void RasterizerStorageGLES3::material_remove_instance_owner(RID p_material, RasterizerScene::InstanceBase *p_instance) {

	Material *material = material_owner.get(  p_material );
	ERR_FAIL_COND(!material);

	Map<RasterizerScene::InstanceBase*,int>::Element *E=material->instance_owners.find(p_instance);
	ERR_FAIL_COND(!E);
	E->get()--;

	if (E->get()==0) {
		material->instance_owners.erase(E);
	}
}



_FORCE_INLINE_ static void _fill_std140_variant_ubo_value(ShaderLanguage::DataType type, const Variant& value, uint8_t *data,bool p_linear_color) {
	switch(type) {
		case ShaderLanguage::TYPE_BOOL: {

			bool v = value;

			GLuint *gui = (GLuint*)data;
			*gui = v ? GL_TRUE : GL_FALSE;
		} break;
		case ShaderLanguage::TYPE_BVEC2: {

			int v = value;
			GLuint *gui = (GLuint*)data;
			gui[0]=v&1 ? GL_TRUE : GL_FALSE;
			gui[1]=v&2 ? GL_TRUE : GL_FALSE;

		} break;
		case ShaderLanguage::TYPE_BVEC3: {

			int v = value;
			GLuint *gui = (GLuint*)data;
			gui[0]=v&1 ? GL_TRUE : GL_FALSE;
			gui[1]=v&2 ? GL_TRUE : GL_FALSE;
			gui[2]=v&4 ? GL_TRUE : GL_FALSE;

		} break;
		case ShaderLanguage::TYPE_BVEC4: {

			int v = value;
			GLuint *gui = (GLuint*)data;
			gui[0]=v&1 ? GL_TRUE : GL_FALSE;
			gui[1]=v&2 ? GL_TRUE : GL_FALSE;
			gui[2]=v&4 ? GL_TRUE : GL_FALSE;
			gui[3]=v&8 ? GL_TRUE : GL_FALSE;

		} break;
		case ShaderLanguage::TYPE_INT: {

			int v = value;
			GLint *gui = (GLint*)data;
			gui[0]=v;

		} break;
		case ShaderLanguage::TYPE_IVEC2: {

			PoolVector<int> iv = value;
			int s = iv.size();
			GLint *gui = (GLint*)data;

			PoolVector<int>::Read r = iv.read();

			for(int i=0;i<2;i++) {
				if (i<s)
					gui[i]=r[i];
				else
					gui[i]=0;

			}

		} break;
		case ShaderLanguage::TYPE_IVEC3: {

			PoolVector<int> iv = value;
			int s = iv.size();
			GLint *gui = (GLint*)data;

			PoolVector<int>::Read r = iv.read();

			for(int i=0;i<3;i++) {
				if (i<s)
					gui[i]=r[i];
				else
					gui[i]=0;

			}
		} break;
		case ShaderLanguage::TYPE_IVEC4: {


			PoolVector<int> iv = value;
			int s = iv.size();
			GLint *gui = (GLint*)data;

			PoolVector<int>::Read r = iv.read();

			for(int i=0;i<4;i++) {
				if (i<s)
					gui[i]=r[i];
				else
					gui[i]=0;

			}
		} break;
		case ShaderLanguage::TYPE_UINT: {

			int v = value;
			GLuint *gui = (GLuint*)data;
			gui[0]=v;

		} break;
		case ShaderLanguage::TYPE_UVEC2: {

			PoolVector<int> iv = value;
			int s = iv.size();
			GLuint *gui = (GLuint*)data;

			PoolVector<int>::Read r = iv.read();

			for(int i=0;i<2;i++) {
				if (i<s)
					gui[i]=r[i];
				else
					gui[i]=0;

			}
		} break;
		case ShaderLanguage::TYPE_UVEC3: {
			PoolVector<int> iv = value;
			int s = iv.size();
			GLuint *gui = (GLuint*)data;

			PoolVector<int>::Read r = iv.read();

			for(int i=0;i<3;i++) {
				if (i<s)
					gui[i]=r[i];
				else
					gui[i]=0;
			}

		} break;
		case ShaderLanguage::TYPE_UVEC4: {
			PoolVector<int> iv = value;
			int s = iv.size();
			GLuint *gui = (GLuint*)data;

			PoolVector<int>::Read r = iv.read();

			for(int i=0;i<4;i++) {
				if (i<s)
					gui[i]=r[i];
				else
					gui[i]=0;
			}
		} break;
		case ShaderLanguage::TYPE_FLOAT: {
			float v = value;
			GLfloat *gui = (GLfloat*)data;
			gui[0]=v;

		} break;
		case ShaderLanguage::TYPE_VEC2: {
			Vector2 v = value;
			GLfloat *gui = (GLfloat*)data;
			gui[0]=v.x;
			gui[1]=v.y;

		} break;
		case ShaderLanguage::TYPE_VEC3: {
			Vector3 v = value;
			GLfloat *gui = (GLfloat*)data;
			gui[0]=v.x;
			gui[1]=v.y;
			gui[2]=v.z;

		} break;
		case ShaderLanguage::TYPE_VEC4: {

			GLfloat *gui = (GLfloat*)data;

			if (value.get_type()==Variant::COLOR) {
				Color v=value;

				if (p_linear_color) {
					v=v.to_linear();
				}

				gui[0]=v.r;
				gui[1]=v.g;
				gui[2]=v.b;
				gui[3]=v.a;
			} else if (value.get_type()==Variant::RECT2) {
				Rect2 v=value;

				gui[0]=v.pos.x;
				gui[1]=v.pos.y;
				gui[2]=v.size.x;
				gui[3]=v.size.y;
			} else if (value.get_type()==Variant::QUAT) {
				Quat v=value;

				gui[0]=v.x;
				gui[1]=v.y;
				gui[2]=v.z;
				gui[3]=v.w;
			} else {
				Plane v=value;

				gui[0]=v.normal.x;
				gui[1]=v.normal.y;
				gui[2]=v.normal.x;
				gui[3]=v.d;

			}
		} break;
		case ShaderLanguage::TYPE_MAT2: {
			Transform2D v = value;
			GLfloat *gui = (GLfloat*)data;

			gui[ 0]=v.elements[0][0];
			gui[ 1]=v.elements[0][1];
			gui[ 2]=v.elements[1][0];
			gui[ 3]=v.elements[1][1];
		} break;
		case ShaderLanguage::TYPE_MAT3: {


			Basis v = value;
			GLfloat *gui = (GLfloat*)data;

			gui[ 0]=v.elements[0][0];
			gui[ 1]=v.elements[1][0];
			gui[ 2]=v.elements[2][0];
			gui[ 3]=0;
			gui[ 4]=v.elements[0][1];
			gui[ 5]=v.elements[1][1];
			gui[ 6]=v.elements[2][1];
			gui[ 7]=0;
			gui[ 8]=v.elements[0][2];
			gui[ 9]=v.elements[1][2];
			gui[10]=v.elements[2][2];
			gui[11]=0;
		} break;
		case ShaderLanguage::TYPE_MAT4: {

			Transform v = value;
			GLfloat *gui = (GLfloat*)data;

			gui[ 0]=v.basis.elements[0][0];
			gui[ 1]=v.basis.elements[1][0];
			gui[ 2]=v.basis.elements[2][0];
			gui[ 3]=0;
			gui[ 4]=v.basis.elements[0][1];
			gui[ 5]=v.basis.elements[1][1];
			gui[ 6]=v.basis.elements[2][1];
			gui[ 7]=0;
			gui[ 8]=v.basis.elements[0][2];
			gui[ 9]=v.basis.elements[1][2];
			gui[10]=v.basis.elements[2][2];
			gui[11]=0;
			gui[12]=v.origin.x;
			gui[13]=v.origin.y;
			gui[14]=v.origin.z;
			gui[15]=1;
		} break;
		default: {}
	}

}

_FORCE_INLINE_ static void _fill_std140_ubo_value(ShaderLanguage::DataType type, const Vector<ShaderLanguage::ConstantNode::Value>& value, uint8_t *data) {

	switch(type) {
		case ShaderLanguage::TYPE_BOOL: {

			GLuint *gui = (GLuint*)data;
			*gui = value[0].boolean ? GL_TRUE : GL_FALSE;
		} break;
		case ShaderLanguage::TYPE_BVEC2: {

			GLuint *gui = (GLuint*)data;
			gui[0]=value[0].boolean ? GL_TRUE : GL_FALSE;
			gui[1]=value[1].boolean ? GL_TRUE : GL_FALSE;

		} break;
		case ShaderLanguage::TYPE_BVEC3: {

			GLuint *gui = (GLuint*)data;
			gui[0]=value[0].boolean ? GL_TRUE : GL_FALSE;
			gui[1]=value[1].boolean ? GL_TRUE : GL_FALSE;
			gui[2]=value[2].boolean ? GL_TRUE : GL_FALSE;

		} break;
		case ShaderLanguage::TYPE_BVEC4: {

			GLuint *gui = (GLuint*)data;
			gui[0]=value[0].boolean ? GL_TRUE : GL_FALSE;
			gui[1]=value[1].boolean ? GL_TRUE : GL_FALSE;
			gui[2]=value[2].boolean ? GL_TRUE : GL_FALSE;
			gui[3]=value[3].boolean ? GL_TRUE : GL_FALSE;

		} break;
		case ShaderLanguage::TYPE_INT: {

			GLint *gui = (GLint*)data;
			gui[0]=value[0].sint;

		} break;
		case ShaderLanguage::TYPE_IVEC2: {

			GLint *gui = (GLint*)data;

			for(int i=0;i<2;i++) {
				gui[i]=value[i].sint;

			}

		} break;
		case ShaderLanguage::TYPE_IVEC3: {

			GLint *gui = (GLint*)data;

			for(int i=0;i<3;i++) {
				gui[i]=value[i].sint;

			}

		} break;
		case ShaderLanguage::TYPE_IVEC4: {

			GLint *gui = (GLint*)data;

			for(int i=0;i<4;i++) {
				gui[i]=value[i].sint;

			}

		} break;
		case ShaderLanguage::TYPE_UINT: {


			GLuint *gui = (GLuint*)data;
			gui[0]=value[0].uint;

		} break;
		case ShaderLanguage::TYPE_UVEC2: {

			GLint *gui = (GLint*)data;

			for(int i=0;i<2;i++) {
				gui[i]=value[i].uint;
			}
		} break;
		case ShaderLanguage::TYPE_UVEC3: {
			GLint *gui = (GLint*)data;

			for(int i=0;i<3;i++) {
				gui[i]=value[i].uint;
			}

		} break;
		case ShaderLanguage::TYPE_UVEC4: {
			GLint *gui = (GLint*)data;

			for(int i=0;i<4;i++) {
				gui[i]=value[i].uint;
			}
		} break;
		case ShaderLanguage::TYPE_FLOAT: {

			GLfloat *gui = (GLfloat*)data;
			gui[0]=value[0].real;

		} break;
		case ShaderLanguage::TYPE_VEC2: {

			GLfloat *gui = (GLfloat*)data;

			for(int i=0;i<2;i++) {
				gui[i]=value[i].real;
			}

		} break;
		case ShaderLanguage::TYPE_VEC3: {

			GLfloat *gui = (GLfloat*)data;

			for(int i=0;i<3;i++) {
				gui[i]=value[i].real;
			}

		} break;
		case ShaderLanguage::TYPE_VEC4: {

			GLfloat *gui = (GLfloat*)data;

			for(int i=0;i<4;i++) {
				gui[i]=value[i].real;
			}
		} break;
		case ShaderLanguage::TYPE_MAT2: {
			GLfloat *gui = (GLfloat*)data;

			for(int i=0;i<2;i++) {
				gui[i]=value[i].real;
			}
		} break;
		case ShaderLanguage::TYPE_MAT3: {



			GLfloat *gui = (GLfloat*)data;

			gui[ 0]=value[0].real;
			gui[ 1]=value[1].real;
			gui[ 2]=value[2].real;
			gui[ 3]=0;
			gui[ 4]=value[3].real;
			gui[ 5]=value[4].real;
			gui[ 6]=value[5].real;
			gui[ 7]=0;
			gui[ 8]=value[6].real;
			gui[ 9]=value[7].real;
			gui[10]=value[8].real;
			gui[11]=0;
		} break;
		case ShaderLanguage::TYPE_MAT4: {

			GLfloat *gui = (GLfloat*)data;

			for(int i=0;i<16;i++) {
				gui[i]=value[i].real;
			}
		} break;
		default: {}
	}

}


_FORCE_INLINE_ static void _fill_std140_ubo_empty(ShaderLanguage::DataType type, uint8_t *data) {

	switch(type) {

		case ShaderLanguage::TYPE_BOOL:
		case ShaderLanguage::TYPE_INT:
		case ShaderLanguage::TYPE_UINT:
		case ShaderLanguage::TYPE_FLOAT: {
			zeromem(data,4);
		} break;
		case ShaderLanguage::TYPE_BVEC2:
		case ShaderLanguage::TYPE_IVEC2:
		case ShaderLanguage::TYPE_UVEC2:
		case ShaderLanguage::TYPE_VEC2: {
			zeromem(data,8);
		} break;
		case ShaderLanguage::TYPE_BVEC3:
		case ShaderLanguage::TYPE_IVEC3:
		case ShaderLanguage::TYPE_UVEC3:
		case ShaderLanguage::TYPE_VEC3:
		case ShaderLanguage::TYPE_BVEC4:
		case ShaderLanguage::TYPE_IVEC4:
		case ShaderLanguage::TYPE_UVEC4:
		case ShaderLanguage::TYPE_VEC4:
		case ShaderLanguage::TYPE_MAT2:{

			zeromem(data,16);
		} break;
		case ShaderLanguage::TYPE_MAT3:{

			zeromem(data,48);
		} break;
		case ShaderLanguage::TYPE_MAT4:{
			zeromem(data,64);
		} break;

		default: {}
	}

}

void RasterizerStorageGLES3::_update_material(Material* material) {

	if (material->dirty_list.in_list())
		_material_dirty_list.remove( &material->dirty_list );


	if (material->shader && material->shader->dirty_list.in_list()) {
		_update_shader(material->shader);
	}
	//update caches

	{
		bool can_cast_shadow = false;
		bool is_animated = false;

		if (material->shader && material->shader->mode==VS::SHADER_SPATIAL) {
			if (!material->shader->spatial.uses_alpha && material->shader->spatial.blend_mode==Shader::Spatial::BLEND_MODE_MIX) {
				can_cast_shadow=true;
			}

			if (material->shader->spatial.uses_discard && material->shader->uses_fragment_time) {
				is_animated=true;
			}

			if (material->shader->spatial.uses_vertex && material->shader->uses_vertex_time) {
				is_animated=true;
			}

		}

		if (can_cast_shadow!=material->can_cast_shadow_cache || is_animated!=material->is_animated_cache) {
			material->can_cast_shadow_cache=can_cast_shadow;
			material->is_animated_cache=is_animated;

			for(Map<Geometry*,int>::Element *E=material->geometry_owners.front();E;E=E->next()) {
				E->key()->material_changed_notify();
			}

			for(Map<RasterizerScene::InstanceBase*,int>::Element *E=material->instance_owners.front();E;E=E->next()) {
				E->key()->base_material_changed();
			}

		}

	}


	//clear ubo if it needs to be cleared
	if (material->ubo_size) {

		if (!material->shader || material->shader->ubo_size!=material->ubo_size) {
			//by by ubo
			glDeleteBuffers(1,&material->ubo_id);
			material->ubo_id=0;
			material->ubo_size=0;
		}
	}

	//create ubo if it needs to be created
	if (material->ubo_size==0 && material->shader && material->shader->ubo_size) {

		glGenBuffers(1, &material->ubo_id);
		glBindBuffer(GL_UNIFORM_BUFFER, material->ubo_id);
		glBufferData(GL_UNIFORM_BUFFER, material->shader->ubo_size, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		material->ubo_size=material->shader->ubo_size;
	}

	//fill up the UBO if it needs to be filled
	if (material->shader && material->ubo_size) {
		uint8_t* local_ubo = (uint8_t*)alloca(material->ubo_size);

		for(Map<StringName,ShaderLanguage::ShaderNode::Uniform>::Element *E=material->shader->uniforms.front();E;E=E->next()) {

			if (E->get().order<0)
				continue; // texture, does not go here

			//regular uniform
			uint8_t *data = &local_ubo[ material->shader->ubo_offsets[E->get().order] ];

			Map<StringName,Variant>::Element *V = material->params.find(E->key());

			if (V) {
				//user provided				
				_fill_std140_variant_ubo_value(E->get().type,V->get(),data,material->shader->mode==VS::SHADER_SPATIAL);

			} else if (E->get().default_value.size()){
				//default value
				_fill_std140_ubo_value(E->get().type,E->get().default_value,data);
				//value=E->get().default_value;
			} else {
				//zero because it was not provided
				_fill_std140_ubo_empty(E->get().type,data);
			}


		}

		glBindBuffer(GL_UNIFORM_BUFFER,material->ubo_id);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, material->ubo_size, local_ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	//set up the texture array, for easy access when it needs to be drawn
	if (material->shader && material->shader->texture_count) {

		material->textures.resize(material->shader->texture_count);

		for(Map<StringName,ShaderLanguage::ShaderNode::Uniform>::Element *E=material->shader->uniforms.front();E;E=E->next()) {

			if (E->get().texture_order<0)
				continue; // not a texture, does not go here

			RID texture;

			Map<StringName,Variant>::Element *V = material->params.find(E->key());
			if (V) {
				texture=V->get();
			}

			if (!texture.is_valid()) {
				Map<StringName,RID>::Element *W = material->shader->default_textures.find(E->key());
				if (W) {
					texture=W->get();
				}
			}

			material->textures[ E->get().texture_order ]=texture;


		}


	} else {
		material->textures.clear();
	}

}

void RasterizerStorageGLES3::_material_add_geometry(RID p_material,Geometry *p_geometry) {

	Material * material = material_owner.getornull(p_material);
	ERR_FAIL_COND(!material);

	Map<Geometry*,int>::Element *I = material->geometry_owners.find(p_geometry);

	if (I) {
		I->get()++;
	} else {
		material->geometry_owners[p_geometry]=1;
	}

}

void RasterizerStorageGLES3::_material_remove_geometry(RID p_material,Geometry *p_geometry) {

	Material * material = material_owner.getornull(p_material);
	ERR_FAIL_COND(!material);

	Map<Geometry*,int>::Element *I = material->geometry_owners.find(p_geometry);
	ERR_FAIL_COND(!I);

	I->get()--;
	if (I->get()==0) {
		material->geometry_owners.erase(I);
	}
}


void RasterizerStorageGLES3::update_dirty_materials() {

	while( _material_dirty_list.first() ) {

		Material *material = _material_dirty_list.first()->self();

		_update_material(material);
	}
}

/* MESH API */

RID RasterizerStorageGLES3::mesh_create(){

	Mesh * mesh = memnew( Mesh );

	return mesh_owner.make_rid(mesh);
}


void RasterizerStorageGLES3::mesh_add_surface(RID p_mesh,uint32_t p_format,VS::PrimitiveType p_primitive,const PoolVector<uint8_t>& p_array,int p_vertex_count,const PoolVector<uint8_t>& p_index_array,int p_index_count,const Rect3& p_aabb,const Vector<PoolVector<uint8_t> >& p_blend_shapes,const Vector<Rect3>& p_bone_aabbs){

	PoolVector<uint8_t> array = p_array;

	Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND(!mesh);

	ERR_FAIL_COND(!(p_format&VS::ARRAY_FORMAT_VERTEX));

	//must have index and bones, both.
	{
		uint32_t bones_weight = VS::ARRAY_FORMAT_BONES|VS::ARRAY_FORMAT_WEIGHTS;
		ERR_EXPLAIN("Array must have both bones and weights in format or none.");
		ERR_FAIL_COND( (p_format&bones_weight) && (p_format&bones_weight)!=bones_weight );
	}


	//bool has_morph = p_blend_shapes.size();

	Surface::Attrib attribs[VS::ARRAY_MAX];

	int stride=0;

	for(int i=0;i<VS::ARRAY_MAX;i++) {

		attribs[i].index=i;

		if (! (p_format&(1<<i) ) ) {
			attribs[i].enabled=false;
			attribs[i].integer=false;
			continue;
		}

		attribs[i].enabled=true;
		attribs[i].offset=stride;
		attribs[i].integer=false;

		switch(i) {

			case VS::ARRAY_VERTEX: {

				if (p_format&VS::ARRAY_FLAG_USE_2D_VERTICES) {
					attribs[i].size=2;
				} else {
					attribs[i].size=(p_format&VS::ARRAY_COMPRESS_VERTEX)?4:3;
				}

				if (p_format&VS::ARRAY_COMPRESS_VERTEX) {
					attribs[i].type=GL_HALF_FLOAT;
					stride+=attribs[i].size*2;
				} else {
					attribs[i].type=GL_FLOAT;
					stride+=attribs[i].size*4;
				}

				attribs[i].normalized=GL_FALSE;

			} break;
			case VS::ARRAY_NORMAL: {

				attribs[i].size=3;

				if (p_format&VS::ARRAY_COMPRESS_NORMAL) {
					attribs[i].type=GL_BYTE;
					stride+=4; //pad extra byte
					attribs[i].normalized=GL_TRUE;
				} else {
					attribs[i].type=GL_FLOAT;
					stride+=12;
					attribs[i].normalized=GL_FALSE;
				}



			} break;
			case VS::ARRAY_TANGENT: {

				attribs[i].size=4;

				if (p_format&VS::ARRAY_COMPRESS_TANGENT) {
					attribs[i].type=GL_BYTE;
					stride+=4;
					attribs[i].normalized=GL_TRUE;
				} else {
					attribs[i].type=GL_FLOAT;
					stride+=16;
					attribs[i].normalized=GL_FALSE;
				}


			} break;
			case VS::ARRAY_COLOR: {

				attribs[i].size=4;

				if (p_format&VS::ARRAY_COMPRESS_COLOR) {
					attribs[i].type=GL_UNSIGNED_BYTE;
					stride+=4;
					attribs[i].normalized=GL_TRUE;
				} else {
					attribs[i].type=GL_FLOAT;
					stride+=16;
					attribs[i].normalized=GL_FALSE;
				}


			} break;
			case VS::ARRAY_TEX_UV: {

				attribs[i].size=2;

				if (p_format&VS::ARRAY_COMPRESS_TEX_UV) {
					attribs[i].type=GL_HALF_FLOAT;
					stride+=4;
				} else {
					attribs[i].type=GL_FLOAT;
					stride+=8;
				}

				attribs[i].normalized=GL_FALSE;


			} break;
			case VS::ARRAY_TEX_UV2: {

				attribs[i].size=2;

				if (p_format&VS::ARRAY_COMPRESS_TEX_UV2) {
					attribs[i].type=GL_HALF_FLOAT;
					stride+=4;
				} else {
					attribs[i].type=GL_FLOAT;
					stride+=8;
				}
				attribs[i].normalized=GL_FALSE;



			} break;
			case VS::ARRAY_BONES: {

				attribs[i].size=4;

				if (p_format&VS::ARRAY_FLAG_USE_16_BIT_BONES) {
					attribs[i].type=GL_UNSIGNED_SHORT;
					stride+=8;
				} else {
					attribs[i].type=GL_UNSIGNED_BYTE;
					stride+=4;
				}

				attribs[i].normalized=GL_FALSE;
				attribs[i].integer=true;



			} break;
			case VS::ARRAY_WEIGHTS: {

				attribs[i].size=4;

				if (p_format&VS::ARRAY_COMPRESS_WEIGHTS) {

					attribs[i].type=GL_UNSIGNED_SHORT;
					stride+=8;
					attribs[i].normalized=GL_TRUE;
				} else {
					attribs[i].type=GL_FLOAT;
					stride+=16;
					attribs[i].normalized=GL_FALSE;
				}

			} break;
			case VS::ARRAY_INDEX: {

				attribs[i].size=1;

				if (p_vertex_count>=(1<<16)) {
					attribs[i].type=GL_UNSIGNED_INT;
					attribs[i].stride=4;
				} else {
					attribs[i].type=GL_UNSIGNED_SHORT;
					attribs[i].stride=2;
				}

				attribs[i].normalized=GL_FALSE;

			} break;

		}
	}

	for(int i=0;i<VS::ARRAY_MAX-1;i++) {
		attribs[i].stride=stride;
	}

	//validate sizes

	int array_size = stride * p_vertex_count;
	int index_array_size=0;

	print_line("desired size: "+itos(array_size)+" vcount "+itos(p_vertex_count)+" should be: "+itos(array.size()+p_vertex_count*2)+" but is "+itos(array.size()));
	if (array.size()!=array_size && array.size()+p_vertex_count*2 == array_size) {
		//old format, convert
		array = PoolVector<uint8_t>();

		array.resize( p_array.size()+p_vertex_count*2 );

		PoolVector<uint8_t>::Write w = array.write();
		PoolVector<uint8_t>::Read r = p_array.read();

		uint16_t *w16 = (uint16_t*)w.ptr();
		const uint16_t *r16 = (uint16_t*)r.ptr();

		uint16_t one = Math::make_half_float(1);

		for(int i=0;i<p_vertex_count;i++) {

			*w16++ = *r16++;
			*w16++ = *r16++;
			*w16++ = *r16++;
			*w16++ = one;
			for(int j=0;j<(stride/2)-4;j++) {
				*w16++ = *r16++;
			}
		}

	}

	ERR_FAIL_COND(array.size()!=array_size);

	if (p_format&VS::ARRAY_FORMAT_INDEX) {

		index_array_size=attribs[VS::ARRAY_INDEX].stride*p_index_count;
	}


	ERR_FAIL_COND(p_index_array.size()!=index_array_size);

	ERR_FAIL_COND(p_blend_shapes.size()!=mesh->blend_shape_count);

	for(int i=0;i<p_blend_shapes.size();i++) {
		ERR_FAIL_COND(p_blend_shapes[i].size()!=array_size);
	}

	//ok all valid, create stuff

	Surface * surface = memnew( Surface );

	surface->active=true;
	surface->array_len=p_vertex_count;
	surface->index_array_len=p_index_count;
	surface->array_byte_size=array.size();
	surface->index_array_byte_size=p_index_array.size();
	surface->primitive=p_primitive;
	surface->mesh=mesh;
	surface->format=p_format;
	surface->skeleton_bone_aabb=p_bone_aabbs;
	surface->skeleton_bone_used.resize(surface->skeleton_bone_aabb.size());
	surface->aabb=p_aabb;
	surface->max_bone=p_bone_aabbs.size();

	for(int i=0;i<surface->skeleton_bone_used.size();i++) {
		if (surface->skeleton_bone_aabb[i].size.x<0 || surface->skeleton_bone_aabb[i].size.y<0 || surface->skeleton_bone_aabb[i].size.z<0) {
			surface->skeleton_bone_used[i]=false;
		} else {
			surface->skeleton_bone_used[i]=true;
		}
	}

	for(int i=0;i<VS::ARRAY_MAX;i++) {
		surface->attribs[i]=attribs[i];
	}

	{

		PoolVector<uint8_t>::Read vr = array.read();

		glGenBuffers(1,&surface->vertex_id);
		glBindBuffer(GL_ARRAY_BUFFER,surface->vertex_id);
		glBufferData(GL_ARRAY_BUFFER,array_size,vr.ptr(),GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0); //unbind


		if (p_format&VS::ARRAY_FORMAT_INDEX) {

			PoolVector<uint8_t>::Read ir = p_index_array.read();

			glGenBuffers(1,&surface->index_id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,surface->index_id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,index_array_size,ir.ptr(),GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0); //unbind


		}

		//generate arrays for faster state switching

		for(int ai=0;ai<2;ai++) {

			if (ai==0) {
				//for normal draw
				glGenVertexArrays(1,&surface->array_id);
				glBindVertexArray(surface->array_id);
				glBindBuffer(GL_ARRAY_BUFFER,surface->vertex_id);
			} else if (ai==1) {
				//for instancing draw (can be changed and no one cares)
				glGenVertexArrays(1,&surface->instancing_array_id);
				glBindVertexArray(surface->instancing_array_id);
				glBindBuffer(GL_ARRAY_BUFFER,surface->vertex_id);
			}


			for(int i=0;i<VS::ARRAY_MAX-1;i++) {

				if (!attribs[i].enabled)
					continue;

				if (attribs[i].integer) {
					glVertexAttribIPointer(attribs[i].index,attribs[i].size,attribs[i].type,attribs[i].stride,((uint8_t*)0)+attribs[i].offset);
				} else {
					glVertexAttribPointer(attribs[i].index,attribs[i].size,attribs[i].type,attribs[i].normalized,attribs[i].stride,((uint8_t*)0)+attribs[i].offset);
				}
				glEnableVertexAttribArray(attribs[i].index);

			}

			if (surface->index_id) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,surface->index_id);
			}

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER,0); //unbind
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		}

	}

	{

		//blend shapes

		for(int i=0;i<p_blend_shapes.size();i++) {

			Surface::BlendShape mt;

			PoolVector<uint8_t>::Read vr = p_blend_shapes[i].read();

			glGenBuffers(1,&mt.vertex_id);
			glBindBuffer(GL_ARRAY_BUFFER,mt.vertex_id);
			glBufferData(GL_ARRAY_BUFFER,array_size,vr.ptr(),GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER,0); //unbind

			glGenVertexArrays(1,&mt.array_id);
			glBindVertexArray(mt.array_id);
			glBindBuffer(GL_ARRAY_BUFFER,mt.vertex_id);

			for(int j=0;j<VS::ARRAY_MAX-1;j++) {

				if (!attribs[j].enabled)
					continue;

				if (attribs[j].integer) {
					glVertexAttribIPointer(attribs[j].index,attribs[j].size,attribs[j].type,attribs[j].stride,((uint8_t*)0)+attribs[j].offset);
				} else {
					glVertexAttribPointer(attribs[j].index,attribs[j].size,attribs[j].type,attribs[j].normalized,attribs[j].stride,((uint8_t*)0)+attribs[j].offset);
				}
				glEnableVertexAttribArray(attribs[j].index);

			}

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER,0); //unbind

			surface->blend_shapes.push_back(mt);

		}
	}

	mesh->surfaces.push_back(surface);
	mesh->instance_change_notify();
}

void RasterizerStorageGLES3::mesh_set_blend_shape_count(RID p_mesh,int p_amount){

	Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND(!mesh);


	ERR_FAIL_COND(mesh->surfaces.size()!=0);
	ERR_FAIL_COND(p_amount<0);

	mesh->blend_shape_count=p_amount;

}
int RasterizerStorageGLES3::mesh_get_blend_shape_count(RID p_mesh) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,0);

	return mesh->blend_shape_count;
}


void RasterizerStorageGLES3::mesh_set_blend_shape_mode(RID p_mesh,VS::BlendShapeMode p_mode){

	Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND(!mesh);

	mesh->blend_shape_mode=p_mode;

}
VS::BlendShapeMode RasterizerStorageGLES3::mesh_get_blend_shape_mode(RID p_mesh) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,VS::BLEND_SHAPE_MODE_NORMALIZED);

	return mesh->blend_shape_mode;
}

void RasterizerStorageGLES3::mesh_surface_set_material(RID p_mesh, int p_surface, RID p_material){

	Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND(!mesh);
	ERR_FAIL_INDEX(p_surface,mesh->surfaces.size());

	if (mesh->surfaces[p_surface]->material==p_material)
		return;

	if (mesh->surfaces[p_surface]->material.is_valid()) {
		_material_remove_geometry(mesh->surfaces[p_surface]->material,mesh->surfaces[p_surface]);
	}

	mesh->surfaces[p_surface]->material=p_material;

	if (mesh->surfaces[p_surface]->material.is_valid()) {
		_material_add_geometry(mesh->surfaces[p_surface]->material,mesh->surfaces[p_surface]);
	}

	mesh->instance_material_change_notify();


}
RID RasterizerStorageGLES3::mesh_surface_get_material(RID p_mesh, int p_surface) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,RID());
	ERR_FAIL_INDEX_V(p_surface,mesh->surfaces.size(),RID());

	return mesh->surfaces[p_surface]->material;
}

int RasterizerStorageGLES3::mesh_surface_get_array_len(RID p_mesh, int p_surface) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,0);
	ERR_FAIL_INDEX_V(p_surface,mesh->surfaces.size(),0);

	return mesh->surfaces[p_surface]->array_len;

}
int RasterizerStorageGLES3::mesh_surface_get_array_index_len(RID p_mesh, int p_surface) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,0);
	ERR_FAIL_INDEX_V(p_surface,mesh->surfaces.size(),0);

	return mesh->surfaces[p_surface]->index_array_len;
}

PoolVector<uint8_t> RasterizerStorageGLES3::mesh_surface_get_array(RID p_mesh, int p_surface) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,PoolVector<uint8_t>());
	ERR_FAIL_INDEX_V(p_surface,mesh->surfaces.size(),PoolVector<uint8_t>());

	Surface *surface = mesh->surfaces[p_surface];

	glBindBuffer(GL_ARRAY_BUFFER,surface->vertex_id);
	void * data = glMapBufferRange(GL_ARRAY_BUFFER,0,surface->array_byte_size,GL_MAP_READ_BIT);

	ERR_FAIL_COND_V(!data,PoolVector<uint8_t>());

	PoolVector<uint8_t> ret;
	ret.resize(surface->array_byte_size);

	{

		PoolVector<uint8_t>::Write w = ret.write();
		copymem(w.ptr(),data,surface->array_byte_size);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);


	return ret;
}

PoolVector<uint8_t> RasterizerStorageGLES3::mesh_surface_get_index_array(RID p_mesh, int p_surface) const {
	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,PoolVector<uint8_t>());
	ERR_FAIL_INDEX_V(p_surface,mesh->surfaces.size(),PoolVector<uint8_t>());

	Surface *surface = mesh->surfaces[p_surface];

	ERR_FAIL_COND_V(surface->index_array_len==0,PoolVector<uint8_t>());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,surface->index_id);
	void * data = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,0,surface->index_array_byte_size,GL_MAP_READ_BIT);

	ERR_FAIL_COND_V(!data,PoolVector<uint8_t>());

	PoolVector<uint8_t> ret;
	ret.resize(surface->index_array_byte_size);

	{

		PoolVector<uint8_t>::Write w = ret.write();
		copymem(w.ptr(),data,surface->index_array_byte_size);
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	return ret;
}


uint32_t RasterizerStorageGLES3::mesh_surface_get_format(RID p_mesh, int p_surface) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);

	ERR_FAIL_COND_V(!mesh,0);
	ERR_FAIL_INDEX_V(p_surface,mesh->surfaces.size(),0);

	return mesh->surfaces[p_surface]->format;

}

VS::PrimitiveType RasterizerStorageGLES3::mesh_surface_get_primitive_type(RID p_mesh, int p_surface) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,VS::PRIMITIVE_MAX);
	ERR_FAIL_INDEX_V(p_surface,mesh->surfaces.size(),VS::PRIMITIVE_MAX);

	return mesh->surfaces[p_surface]->primitive;
}

Rect3 RasterizerStorageGLES3::mesh_surface_get_aabb(RID p_mesh, int p_surface) const {

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,Rect3());
	ERR_FAIL_INDEX_V(p_surface,mesh->surfaces.size(),Rect3());

	return mesh->surfaces[p_surface]->aabb;


}
Vector<PoolVector<uint8_t> > RasterizerStorageGLES3::mesh_surface_get_blend_shapes(RID p_mesh, int p_surface) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,Vector<PoolVector<uint8_t> >());
	ERR_FAIL_INDEX_V(p_surface,mesh->surfaces.size(),Vector<PoolVector<uint8_t> >());

	Vector<PoolVector<uint8_t> > bsarr;

	for(int i=0;i<mesh->surfaces[p_surface]->blend_shapes.size();i++) {

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh->surfaces[p_surface]->blend_shapes[i].vertex_id);
		void * data = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,0,mesh->surfaces[p_surface]->array_byte_size,GL_MAP_READ_BIT);

		ERR_FAIL_COND_V(!data,Vector<PoolVector<uint8_t> >());

		PoolVector<uint8_t> ret;
		ret.resize(mesh->surfaces[p_surface]->array_byte_size);

		{

			PoolVector<uint8_t>::Write w = ret.write();
			copymem(w.ptr(),data,mesh->surfaces[p_surface]->array_byte_size);
		}

		bsarr.push_back(ret);

		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	}

	return bsarr;

}
Vector<Rect3> RasterizerStorageGLES3::mesh_surface_get_skeleton_aabb(RID p_mesh, int p_surface) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,Vector<Rect3 >());
	ERR_FAIL_INDEX_V(p_surface,mesh->surfaces.size(),Vector<Rect3 >());

	return mesh->surfaces[p_surface]->skeleton_bone_aabb;

}


void RasterizerStorageGLES3::mesh_remove_surface(RID p_mesh, int p_surface){

	Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND(!mesh);
	ERR_FAIL_INDEX(p_surface,mesh->surfaces.size());

	Surface *surface = mesh->surfaces[p_surface];

	if (surface->material.is_valid()) {
		_material_remove_geometry(surface->material,mesh->surfaces[p_surface]);
	}

	glDeleteBuffers(1,&surface->vertex_id);
	if (surface->index_id) {
		glDeleteBuffers(1,&surface->index_id);
	}

	glDeleteVertexArrays(1,&surface->array_id);

	for(int i=0;i<surface->blend_shapes.size();i++) {

		glDeleteBuffers(1,&surface->blend_shapes[i].vertex_id);
		glDeleteVertexArrays(1,&surface->blend_shapes[i].array_id);
	}

	mesh->instance_material_change_notify();

	memdelete(surface);

	mesh->surfaces.remove(p_surface);

	mesh->instance_change_notify();
}
int RasterizerStorageGLES3::mesh_get_surface_count(RID p_mesh) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,0);
	return mesh->surfaces.size();

}

void RasterizerStorageGLES3::mesh_set_custom_aabb(RID p_mesh,const Rect3& p_aabb){

	Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND(!mesh);

	mesh->custom_aabb=p_aabb;
}
Rect3 RasterizerStorageGLES3::mesh_get_custom_aabb(RID p_mesh) const{

	const Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND_V(!mesh,Rect3());

	return mesh->custom_aabb;

}

Rect3 RasterizerStorageGLES3::mesh_get_aabb(RID p_mesh,RID p_skeleton) const{

	Mesh *mesh = mesh_owner.get( p_mesh );
	ERR_FAIL_COND_V(!mesh,Rect3());

	if (mesh->custom_aabb!=Rect3())
		return mesh->custom_aabb;

	Skeleton *sk=NULL;
	if (p_skeleton.is_valid())
		sk=skeleton_owner.get(p_skeleton);

	Rect3 aabb;

	if (sk && sk->size!=0) {


		for (int i=0;i<mesh->surfaces.size();i++) {

			Rect3 laabb;
			if (mesh->surfaces[i]->format&VS::ARRAY_FORMAT_BONES && mesh->surfaces[i]->skeleton_bone_aabb.size()) {


				int bs = mesh->surfaces[i]->skeleton_bone_aabb.size();
				const Rect3 *skbones = mesh->surfaces[i]->skeleton_bone_aabb.ptr();
				const bool *skused = mesh->surfaces[i]->skeleton_bone_used.ptr();

				int sbs = sk->size;
				ERR_CONTINUE(bs>sbs);
				float *skb = sk->bones.ptr();



				bool first=true;
				if (sk->use_2d) {
					for(int j=0;j<bs;j++) {

						if (!skused[j])
							continue;

						float *dataptr = &skb[8*j];

						Transform mtx;

						mtx.basis.elements[0][0]=dataptr[ 0];
						mtx.basis.elements[0][1]=dataptr[ 1];
						mtx.origin[0]=dataptr[ 3];
						mtx.basis.elements[1][0]=dataptr[ 4];
						mtx.basis.elements[1][1]=dataptr[ 5];
						mtx.origin[1]=dataptr[ 7];

						Rect3 baabb = mtx.xform( skbones[j] );
						if (first) {
							laabb=baabb;
							first=false;
						} else {
							laabb.merge_with(baabb);
						}
					}
				} else {
					for(int j=0;j<bs;j++) {

						if (!skused[j])
							continue;

						float *dataptr = &skb[12*j];

						Transform mtx;
						mtx.basis.elements[0][0]=dataptr[ 0];
						mtx.basis.elements[0][1]=dataptr[ 1];
						mtx.basis.elements[0][2]=dataptr[ 2];
						mtx.origin.x=dataptr[ 3];
						mtx.basis.elements[1][0]=dataptr[ 4];
						mtx.basis.elements[1][1]=dataptr[ 5];
						mtx.basis.elements[1][2]=dataptr[ 6];
						mtx.origin.y=dataptr[ 7];
						mtx.basis.elements[2][0]=dataptr[ 8];
						mtx.basis.elements[2][1]=dataptr[ 9];
						mtx.basis.elements[2][2]=dataptr[10];
						mtx.origin.z=dataptr[11];

						Rect3 baabb = mtx.xform ( skbones[j] );
						if (first) {
							laabb=baabb;
							first=false;
						} else {
							laabb.merge_with(baabb);
						}
					}
				}

			} else {

				laabb=mesh->surfaces[i]->aabb;
			}

			if (i==0)
				aabb=laabb;
			else
				aabb.merge_with(laabb);
		}
	} else {

		for (int i=0;i<mesh->surfaces.size();i++) {

			if (i==0)
				aabb=mesh->surfaces[i]->aabb;
			else
				aabb.merge_with(mesh->surfaces[i]->aabb);
		}

	}

	return aabb;

}
void RasterizerStorageGLES3::mesh_clear(RID p_mesh){

	Mesh *mesh = mesh_owner.getornull(p_mesh);
	ERR_FAIL_COND(!mesh);

	while(mesh->surfaces.size()) {
		mesh_remove_surface(p_mesh,0);
	}
}

void RasterizerStorageGLES3::mesh_render_blend_shapes(Surface *s, float *p_weights) {

	glBindVertexArray(s->array_id);

	BlendShapeShaderGLES3::Conditionals cond[VS::ARRAY_MAX-1]={
		BlendShapeShaderGLES3::ENABLE_NORMAL, //will be ignored
		BlendShapeShaderGLES3::ENABLE_NORMAL,
		BlendShapeShaderGLES3::ENABLE_TANGENT,
		BlendShapeShaderGLES3::ENABLE_COLOR,
		BlendShapeShaderGLES3::ENABLE_UV,
		BlendShapeShaderGLES3::ENABLE_UV2,
		BlendShapeShaderGLES3::ENABLE_SKELETON,
		BlendShapeShaderGLES3::ENABLE_SKELETON,
	};

	int stride=0;

	if (s->format&VS::ARRAY_FLAG_USE_2D_VERTICES) {
		stride=2*4;
	} else {
		stride=3*4;
	}

	static const int sizes[VS::ARRAY_MAX-1]={
		3*4,
		3*4,
		4*4,
		4*4,
		2*4,
		2*4,
		4*4,
		4*4
	};

	for(int i=1;i<VS::ARRAY_MAX-1;i++) {
		shaders.blend_shapes.set_conditional(cond[i],s->format&(1<<i)); //enable conditional for format
		if (s->format&(1<<i)) {
			stride+=sizes[i];
		}
	}


	//copy all first
	float base_weight=1.0;

	int mtc = s->blend_shapes.size();

	if (s->mesh->blend_shape_mode==VS::BLEND_SHAPE_MODE_NORMALIZED) {

		for(int i=0;i<mtc;i++) {
			base_weight-=p_weights[i];
		}
	}



	shaders.blend_shapes.set_conditional(BlendShapeShaderGLES3::ENABLE_BLEND,false); //first pass does not blend
	shaders.blend_shapes.set_conditional(BlendShapeShaderGLES3::USE_2D_VERTEX,s->format&VS::ARRAY_FLAG_USE_2D_VERTICES); //use 2D vertices if needed

	shaders.blend_shapes.bind();

	shaders.blend_shapes.set_uniform(BlendShapeShaderGLES3::BLEND_AMOUNT,base_weight);
	glEnable(GL_RASTERIZER_DISCARD);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, resources.transform_feedback_buffers[0]);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS,0,s->array_len);
	glEndTransformFeedback();


	shaders.blend_shapes.set_conditional(BlendShapeShaderGLES3::ENABLE_BLEND,true); //first pass does not blend
	shaders.blend_shapes.bind();

	for(int ti=0;ti<mtc;ti++) {
		float weight = p_weights[ti];

		if (weight<0.001) //not bother with this one
			continue;

		glBindVertexArray(s->blend_shapes[ti].array_id);
		glBindBuffer(GL_ARRAY_BUFFER, resources.transform_feedback_buffers[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, resources.transform_feedback_buffers[1]);

		shaders.blend_shapes.set_uniform(BlendShapeShaderGLES3::BLEND_AMOUNT,weight);

		int ofs=0;
		for(int i=0;i<VS::ARRAY_MAX-1;i++) {

			if (s->format&(1<<i)) {
				glEnableVertexAttribArray(i+8);
				switch(i) {

					case VS::ARRAY_VERTEX: {
						if (s->format&VS::ARRAY_FLAG_USE_2D_VERTICES) {
							glVertexAttribPointer(i+8,2,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
							ofs+=2*4;
						} else {
							glVertexAttribPointer(i+8,3,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
							ofs+=3*4;
						}
					} break;
					case VS::ARRAY_NORMAL: {
						glVertexAttribPointer(i+8,3,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
						ofs+=3*4;
					} break;
					case VS::ARRAY_TANGENT: {
						glVertexAttribPointer(i+8,4,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
						ofs+=4*4;

					} break;
					case VS::ARRAY_COLOR: {
						glVertexAttribPointer(i+8,4,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
						ofs+=4*4;

					} break;
					case VS::ARRAY_TEX_UV: {
						glVertexAttribPointer(i+8,2,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
						ofs+=2*4;

					} break;
					case VS::ARRAY_TEX_UV2: {
						glVertexAttribPointer(i+8,2,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
						ofs+=2*4;

					} break;
					case VS::ARRAY_BONES: {
						glVertexAttribIPointer(i+8,4,GL_UNSIGNED_INT,stride,((uint8_t*)0)+ofs);
						ofs+=4*4;

					} break;
					case VS::ARRAY_WEIGHTS: {
						glVertexAttribPointer(i+8,4,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
						ofs+=4*4;

					} break;
				}

			} else {
				glDisableVertexAttribArray(i+8);
			}
		}

		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS,0,s->array_len);
		glEndTransformFeedback();


		SWAP(resources.transform_feedback_buffers[0],resources.transform_feedback_buffers[1]);

	}

	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);


	glBindVertexArray(resources.transform_feedback_array);
	glBindBuffer(GL_ARRAY_BUFFER, resources.transform_feedback_buffers[0]);

	int ofs=0;
	for(int i=0;i<VS::ARRAY_MAX-1;i++) {

		if (s->format&(1<<i)) {
			glEnableVertexAttribArray(i);
			switch(i) {

				case VS::ARRAY_VERTEX: {
					if (s->format&VS::ARRAY_FLAG_USE_2D_VERTICES) {
						glVertexAttribPointer(i,2,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
						ofs+=2*4;
					} else {
						glVertexAttribPointer(i,3,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
						ofs+=3*4;
					}
				} break;
				case VS::ARRAY_NORMAL: {
					glVertexAttribPointer(i,3,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
					ofs+=3*4;
				} break;
				case VS::ARRAY_TANGENT: {
					glVertexAttribPointer(i,4,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
					ofs+=4*4;

				} break;
				case VS::ARRAY_COLOR: {
					glVertexAttribPointer(i,4,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
					ofs+=4*4;

				} break;
				case VS::ARRAY_TEX_UV: {
					glVertexAttribPointer(i,2,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
					ofs+=2*4;

				} break;
				case VS::ARRAY_TEX_UV2: {
					glVertexAttribPointer(i,2,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
					ofs+=2*4;

				} break;
				case VS::ARRAY_BONES: {
					glVertexAttribIPointer(i,4,GL_UNSIGNED_INT,stride,((uint8_t*)0)+ofs);
					ofs+=4*4;

				} break;
				case VS::ARRAY_WEIGHTS: {
					glVertexAttribPointer(i,4,GL_FLOAT,GL_FALSE,stride,((uint8_t*)0)+ofs);
					ofs+=4*4;

				} break;
			}

		} else {
			glDisableVertexAttribArray(i);
		}
	}

	if (s->index_array_len) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,s->index_id);
	}

}

/* MULTIMESH API */


RID RasterizerStorageGLES3::multimesh_create(){

	MultiMesh *multimesh = memnew( MultiMesh );
	return multimesh_owner.make_rid(multimesh);
}

void RasterizerStorageGLES3::multimesh_allocate(RID p_multimesh, int p_instances, VS::MultimeshTransformFormat p_transform_format, VS::MultimeshColorFormat p_color_format){

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND(!multimesh);

	if (multimesh->size==p_instances && multimesh->transform_format==p_transform_format && multimesh->color_format==p_color_format)
		return;

	if (multimesh->buffer) {
		glDeleteBuffers(1,&multimesh->buffer);
		multimesh->data.resize(0);
	}

	multimesh->size=p_instances;
	multimesh->transform_format=p_transform_format;
	multimesh->color_format=p_color_format;

	if (multimesh->size) {

		if (multimesh->transform_format==VS::MULTIMESH_TRANSFORM_2D) {
			multimesh->xform_floats=8;
		} else {
			multimesh->xform_floats=12;

		}

		if (multimesh->color_format==VS::MULTIMESH_COLOR_NONE) {
			multimesh->color_floats=0;
		} else if (multimesh->color_format==VS::MULTIMESH_COLOR_8BIT) {
			multimesh->color_floats=1;
		} else if (multimesh->color_format==VS::MULTIMESH_COLOR_FLOAT) {
			multimesh->color_floats=4;
		}

		int format_floats = multimesh->color_floats+multimesh->xform_floats;
		multimesh->data.resize(format_floats*p_instances);
		for(int i=0;i<p_instances;i+=format_floats) {

			int color_from=0;

			if (multimesh->transform_format==VS::MULTIMESH_TRANSFORM_2D) {
				multimesh->data[i+0]=1.0;
				multimesh->data[i+1]=0.0;
				multimesh->data[i+2]=0.0;
				multimesh->data[i+3]=0.0;
				multimesh->data[i+4]=0.0;
				multimesh->data[i+5]=1.0;
				multimesh->data[i+6]=0.0;
				multimesh->data[i+7]=0.0;
				color_from=8;
			} else {
				multimesh->data[i+0]=1.0;
				multimesh->data[i+1]=0.0;
				multimesh->data[i+2]=0.0;
				multimesh->data[i+3]=0.0;
				multimesh->data[i+4]=0.0;
				multimesh->data[i+5]=1.0;
				multimesh->data[i+6]=0.0;
				multimesh->data[i+7]=0.0;
				multimesh->data[i+8]=0.0;
				multimesh->data[i+9]=0.0;
				multimesh->data[i+10]=1.0;
				multimesh->data[i+11]=0.0;
				color_from=12;
			}

			if (multimesh->color_format==VS::MULTIMESH_COLOR_NONE) {
				//none
			} else if (multimesh->color_format==VS::MULTIMESH_COLOR_8BIT) {

				union {
					uint32_t colu;
					float colf;
				} cu;

				cu.colu=0xFFFFFFFF;
				multimesh->data[i+color_from+0]=cu.colf;

			} else if (multimesh->color_format==VS::MULTIMESH_COLOR_FLOAT) {
				multimesh->data[i+color_from+0]=1.0;
				multimesh->data[i+color_from+1]=1.0;
				multimesh->data[i+color_from+2]=1.0;
				multimesh->data[i+color_from+3]=1.0;
			}
		}

		glGenBuffers(1,&multimesh->buffer);
		glBindBuffer(GL_ARRAY_BUFFER,multimesh->buffer);
		glBufferData(GL_ARRAY_BUFFER,multimesh->data.size()*sizeof(float),NULL,GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);

	}

	multimesh->dirty_data=true;
	multimesh->dirty_aabb=true;

	if (!multimesh->update_list.in_list()) {
		multimesh_update_list.add(&multimesh->update_list);
	}

}

int RasterizerStorageGLES3::multimesh_get_instance_count(RID p_multimesh) const{

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND_V(!multimesh,0);

	return multimesh->size;
}

void RasterizerStorageGLES3::multimesh_set_mesh(RID p_multimesh,RID p_mesh){

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND(!multimesh);

	multimesh->mesh=p_mesh;

	multimesh->dirty_aabb=true;

	if (!multimesh->update_list.in_list()) {
		multimesh_update_list.add(&multimesh->update_list);
	}
}

void RasterizerStorageGLES3::multimesh_instance_set_transform(RID p_multimesh,int p_index,const Transform& p_transform){

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND(!multimesh);
	ERR_FAIL_INDEX(p_index,multimesh->size);
	ERR_FAIL_COND(multimesh->transform_format==VS::MULTIMESH_TRANSFORM_2D);

	int stride = multimesh->color_floats+multimesh->xform_floats;
	float *dataptr=&multimesh->data[stride*p_index];

	dataptr[ 0]=p_transform.basis.elements[0][0];
	dataptr[ 1]=p_transform.basis.elements[0][1];
	dataptr[ 2]=p_transform.basis.elements[0][2];
	dataptr[ 3]=p_transform.origin.x;
	dataptr[ 4]=p_transform.basis.elements[1][0];
	dataptr[ 5]=p_transform.basis.elements[1][1];
	dataptr[ 6]=p_transform.basis.elements[1][2];
	dataptr[ 7]=p_transform.origin.y;
	dataptr[ 8]=p_transform.basis.elements[2][0];
	dataptr[ 9]=p_transform.basis.elements[2][1];
	dataptr[10]=p_transform.basis.elements[2][2];
	dataptr[11]=p_transform.origin.z;

	multimesh->dirty_data=true;
	multimesh->dirty_aabb=true;

	if (!multimesh->update_list.in_list()) {
		multimesh_update_list.add(&multimesh->update_list);
	}
}

void RasterizerStorageGLES3::multimesh_instance_set_transform_2d(RID p_multimesh,int p_index,const Transform2D& p_transform){

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND(!multimesh);
	ERR_FAIL_INDEX(p_index,multimesh->size);
	ERR_FAIL_COND(multimesh->transform_format==VS::MULTIMESH_TRANSFORM_3D);

	int stride = multimesh->color_floats+multimesh->xform_floats;
	float *dataptr=&multimesh->data[stride*p_index];

	dataptr[ 0]=p_transform.elements[0][0];
	dataptr[ 1]=p_transform.elements[1][0];
	dataptr[ 2]=0;
	dataptr[ 3]=p_transform.elements[2][0];
	dataptr[ 4]=p_transform.elements[0][1];
	dataptr[ 5]=p_transform.elements[1][1];
	dataptr[ 6]=0;
	dataptr[ 7]=p_transform.elements[2][1];

	multimesh->dirty_data=true;
	multimesh->dirty_aabb=true;

	if (!multimesh->update_list.in_list()) {
		multimesh_update_list.add(&multimesh->update_list);
	}
}
void RasterizerStorageGLES3::multimesh_instance_set_color(RID p_multimesh,int p_index,const Color& p_color){

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND(!multimesh);
	ERR_FAIL_INDEX(p_index,multimesh->size);
	ERR_FAIL_COND(multimesh->color_format==VS::MULTIMESH_COLOR_NONE);

	int stride = multimesh->color_floats+multimesh->xform_floats;
	float *dataptr=&multimesh->data[stride*p_index+multimesh->xform_floats];

	if (multimesh->color_format==VS::MULTIMESH_COLOR_8BIT) {

		uint8_t *data8=(uint8_t*)dataptr;
		data8[0]=CLAMP(p_color.r*255.0,0,255);
		data8[1]=CLAMP(p_color.g*255.0,0,255);
		data8[2]=CLAMP(p_color.b*255.0,0,255);
		data8[3]=CLAMP(p_color.a*255.0,0,255);

	} else if (multimesh->color_format==VS::MULTIMESH_COLOR_FLOAT) {
		dataptr[ 0]=p_color.r;
		dataptr[ 1]=p_color.g;
		dataptr[ 2]=p_color.b;
		dataptr[ 3]=p_color.a;
	}


	multimesh->dirty_data=true;
	multimesh->dirty_aabb=true;

	if (!multimesh->update_list.in_list()) {
		multimesh_update_list.add(&multimesh->update_list);
	}
}

RID RasterizerStorageGLES3::multimesh_get_mesh(RID p_multimesh) const{

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND_V(!multimesh,RID());

	return multimesh->mesh;
}


Transform RasterizerStorageGLES3::multimesh_instance_get_transform(RID p_multimesh,int p_index) const{

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND_V(!multimesh,Transform());
	ERR_FAIL_INDEX_V(p_index,multimesh->size,Transform());
	ERR_FAIL_COND_V(multimesh->transform_format==VS::MULTIMESH_TRANSFORM_2D,Transform());

	int stride = multimesh->color_floats+multimesh->xform_floats;
	float *dataptr=&multimesh->data[stride*p_index];

	Transform xform;

	xform.basis.elements[0][0]=dataptr[ 0];
	xform.basis.elements[0][1]=dataptr[ 1];
	xform.basis.elements[0][2]=dataptr[ 2];
	xform.origin.x=dataptr[ 3];
	xform.basis.elements[1][0]=dataptr[ 4];
	xform.basis.elements[1][1]=dataptr[ 5];
	xform.basis.elements[1][2]=dataptr[ 6];
	xform.origin.y=dataptr[ 7];
	xform.basis.elements[2][0]=dataptr[ 8];
	xform.basis.elements[2][1]=dataptr[ 9];
	xform.basis.elements[2][2]=dataptr[10];
	xform.origin.z=dataptr[11];

	return xform;
}
Transform2D RasterizerStorageGLES3::multimesh_instance_get_transform_2d(RID p_multimesh,int p_index) const{

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND_V(!multimesh,Transform2D());
	ERR_FAIL_INDEX_V(p_index,multimesh->size,Transform2D());
	ERR_FAIL_COND_V(multimesh->transform_format==VS::MULTIMESH_TRANSFORM_3D,Transform2D());

	int stride = multimesh->color_floats+multimesh->xform_floats;
	float *dataptr=&multimesh->data[stride*p_index];

	Transform2D xform;

	xform.elements[0][0]=dataptr[ 0];
	xform.elements[1][0]=dataptr[ 1];
	xform.elements[2][0]=dataptr[ 3];
	xform.elements[0][1]=dataptr[ 4];
	xform.elements[1][1]=dataptr[ 5];
	xform.elements[2][1]=dataptr[ 7];

	return xform;
}

Color RasterizerStorageGLES3::multimesh_instance_get_color(RID p_multimesh,int p_index) const{

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND_V(!multimesh,Color());
	ERR_FAIL_INDEX_V(p_index,multimesh->size,Color());
	ERR_FAIL_COND_V(multimesh->color_format==VS::MULTIMESH_COLOR_NONE,Color());

	int stride = multimesh->color_floats+multimesh->xform_floats;
	float *dataptr=&multimesh->data[stride*p_index+multimesh->color_floats];

	if (multimesh->color_format==VS::MULTIMESH_COLOR_8BIT) {
		union {
			uint32_t colu;
			float colf;
		} cu;

		return Color::hex(BSWAP32(cu.colu));

	} else if (multimesh->color_format==VS::MULTIMESH_COLOR_FLOAT) {
		Color c;
		c.r=dataptr[ 0];
		c.g=dataptr[ 1];
		c.b=dataptr[ 2];
		c.a=dataptr[ 3];

		return c;
	}

	return Color();

}

void RasterizerStorageGLES3::multimesh_set_visible_instances(RID p_multimesh,int p_visible){

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND(!multimesh);

	multimesh->visible_instances=p_visible;
}
int RasterizerStorageGLES3::multimesh_get_visible_instances(RID p_multimesh) const{

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND_V(!multimesh,-1);

	return multimesh->visible_instances;
}

Rect3 RasterizerStorageGLES3::multimesh_get_aabb(RID p_multimesh) const{

	MultiMesh *multimesh = multimesh_owner.getornull(p_multimesh);
	ERR_FAIL_COND_V(!multimesh,Rect3());

	const_cast<RasterizerStorageGLES3*>(this)->update_dirty_multimeshes(); //update pending AABBs

	return multimesh->aabb;
}

void RasterizerStorageGLES3::update_dirty_multimeshes() {

	while(multimesh_update_list.first()) {

		MultiMesh *multimesh = multimesh_update_list.first()->self();

		if (multimesh->size && multimesh->dirty_data) {


			glBindBuffer(GL_ARRAY_BUFFER,multimesh->buffer);
			glBufferSubData(GL_ARRAY_BUFFER,0,multimesh->data.size()*sizeof(float),multimesh->data.ptr());
			glBindBuffer(GL_ARRAY_BUFFER,0);


		}



		if (multimesh->size && multimesh->dirty_aabb) {

			Rect3 mesh_aabb;

			if (multimesh->mesh.is_valid()) {
				mesh_aabb=mesh_get_aabb(multimesh->mesh,RID());
			} else {
				mesh_aabb.size+=Vector3(0.001,0.001,0.001);
			}

			int stride=multimesh->color_floats+multimesh->xform_floats;
			int count = multimesh->data.size();
			float *data=multimesh->data.ptr();

			Rect3 aabb;

			if (multimesh->transform_format==VS::MULTIMESH_TRANSFORM_2D) {

				for(int i=0;i<count;i+=stride) {

					float *dataptr=&data[i];
					Transform xform;
					xform.basis[0][0]=dataptr[ 0];
					xform.basis[0][1]=dataptr[ 1];
					xform.origin[0]=dataptr[ 3];
					xform.basis[1][0]=dataptr[ 4];
					xform.basis[1][1]=dataptr[ 5];
					xform.origin[1]=dataptr[ 7];

					Rect3 laabb = xform.xform(mesh_aabb);
					if (i==0)
						aabb=laabb;
					else
						aabb.merge_with(laabb);
				}
			} else {

				for(int i=0;i<count;i+=stride) {

					float *dataptr=&data[i];
					Transform xform;

					xform.basis.elements[0][0]=dataptr[ 0];
					xform.basis.elements[0][1]=dataptr[ 1];
					xform.basis.elements[0][2]=dataptr[ 2];
					xform.origin.x=dataptr[ 3];
					xform.basis.elements[1][0]=dataptr[ 4];
					xform.basis.elements[1][1]=dataptr[ 5];
					xform.basis.elements[1][2]=dataptr[ 6];
					xform.origin.y=dataptr[ 7];
					xform.basis.elements[2][0]=dataptr[ 8];
					xform.basis.elements[2][1]=dataptr[ 9];
					xform.basis.elements[2][2]=dataptr[10];
					xform.origin.z=dataptr[11];

					Rect3 laabb = xform.xform(mesh_aabb);
					if (i==0)
						aabb=laabb;
					else
						aabb.merge_with(laabb);
				}
			}

			multimesh->aabb=aabb;
		}
		multimesh->dirty_aabb=false;
		multimesh->dirty_data=false;

		multimesh->instance_change_notify();

		multimesh_update_list.remove(multimesh_update_list.first());
	}
}

/* IMMEDIATE API */


RID RasterizerStorageGLES3::immediate_create() {

	Immediate *im = memnew( Immediate );
	return immediate_owner.make_rid(im);

}

void RasterizerStorageGLES3::immediate_begin(RID p_immediate, VS::PrimitiveType p_rimitive, RID p_texture){

	Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND(!im);
	ERR_FAIL_COND(im->building);

	Immediate::Chunk ic;
	ic.texture=p_texture;
	ic.primitive=p_rimitive;
	im->chunks.push_back(ic);
	im->mask=0;
	im->building=true;


}
void RasterizerStorageGLES3::immediate_vertex(RID p_immediate,const Vector3& p_vertex){

	Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND(!im);
	ERR_FAIL_COND(!im->building);

	Immediate::Chunk *c = &im->chunks.back()->get();


	if (c->vertices.empty() && im->chunks.size()==1) {

		im->aabb.pos=p_vertex;
		im->aabb.size=Vector3();
	} else {
		im->aabb.expand_to(p_vertex);
	}

	if (im->mask&VS::ARRAY_FORMAT_NORMAL)
		c->normals.push_back(chunk_normal);
	if (im->mask&VS::ARRAY_FORMAT_TANGENT)
		c->tangents.push_back(chunk_tangent);
	if (im->mask&VS::ARRAY_FORMAT_COLOR)
		c->colors.push_back(chunk_color);
	if (im->mask&VS::ARRAY_FORMAT_TEX_UV)
		c->uvs.push_back(chunk_uv);
	if (im->mask&VS::ARRAY_FORMAT_TEX_UV2)
		c->uvs2.push_back(chunk_uv2);
	im->mask|=VS::ARRAY_FORMAT_VERTEX;
	c->vertices.push_back(p_vertex);

}


void RasterizerStorageGLES3::immediate_normal(RID p_immediate,const Vector3& p_normal){

	Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND(!im);
	ERR_FAIL_COND(!im->building);

	im->mask|=VS::ARRAY_FORMAT_NORMAL;
	chunk_normal=p_normal;

}
void RasterizerStorageGLES3::immediate_tangent(RID p_immediate,const Plane& p_tangent){

	Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND(!im);
	ERR_FAIL_COND(!im->building);

	im->mask|=VS::ARRAY_FORMAT_TANGENT;
	chunk_tangent=p_tangent;

}
void RasterizerStorageGLES3::immediate_color(RID p_immediate,const Color& p_color){

	Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND(!im);
	ERR_FAIL_COND(!im->building);

	im->mask|=VS::ARRAY_FORMAT_COLOR;
	chunk_color=p_color;

}
void RasterizerStorageGLES3::immediate_uv(RID p_immediate,const Vector2& tex_uv){

	Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND(!im);
	ERR_FAIL_COND(!im->building);

	im->mask|=VS::ARRAY_FORMAT_TEX_UV;
	chunk_uv=tex_uv;

}
void RasterizerStorageGLES3::immediate_uv2(RID p_immediate,const Vector2& tex_uv){

	Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND(!im);
	ERR_FAIL_COND(!im->building);

	im->mask|=VS::ARRAY_FORMAT_TEX_UV2;
	chunk_uv2=tex_uv;

}

void RasterizerStorageGLES3::immediate_end(RID p_immediate){

	Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND(!im);
	ERR_FAIL_COND(!im->building);

	im->building=false;

	im->instance_change_notify();

}
void RasterizerStorageGLES3::immediate_clear(RID p_immediate) {

	Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND(!im);
	ERR_FAIL_COND(im->building);

	im->chunks.clear();
	im->instance_change_notify();

}

Rect3 RasterizerStorageGLES3::immediate_get_aabb(RID p_immediate) const {

	Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND_V(!im,Rect3());
	return im->aabb;
}

void RasterizerStorageGLES3::immediate_set_material(RID p_immediate,RID p_material) {

	Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND(!im);
	im->material=p_material;
	im->instance_material_change_notify();

}

RID RasterizerStorageGLES3::immediate_get_material(RID p_immediate) const {

	const Immediate *im = immediate_owner.get(p_immediate);
	ERR_FAIL_COND_V(!im,RID());
	return im->material;

}

/* SKELETON API */

RID RasterizerStorageGLES3::skeleton_create(){

	Skeleton *skeleton = memnew( Skeleton );
	return skeleton_owner.make_rid(skeleton);
}

void RasterizerStorageGLES3::skeleton_allocate(RID p_skeleton,int p_bones,bool p_2d_skeleton){

	Skeleton *skeleton = skeleton_owner.getornull(p_skeleton);
	ERR_FAIL_COND(!skeleton);
	ERR_FAIL_COND(p_bones<0);

	if (skeleton->size==p_bones && skeleton->use_2d==p_2d_skeleton)
		return;

	if (skeleton->ubo) {
		glDeleteBuffers(1,&skeleton->ubo);
		skeleton->ubo=0;
	}

	skeleton->size=p_bones;
	if (p_2d_skeleton) {
		skeleton->bones.resize(p_bones*8);
		for(int i=0;i<skeleton->bones.size();i+=8) {
			skeleton->bones[i+0]=1;
			skeleton->bones[i+1]=0;
			skeleton->bones[i+2]=0;
			skeleton->bones[i+3]=0;
			skeleton->bones[i+4]=0;
			skeleton->bones[i+5]=1;
			skeleton->bones[i+6]=0;
			skeleton->bones[i+7]=0;
		}

	} else {
		skeleton->bones.resize(p_bones*12);
		for(int i=0;i<skeleton->bones.size();i+=12) {
			skeleton->bones[i+0]=1;
			skeleton->bones[i+1]=0;
			skeleton->bones[i+2]=0;
			skeleton->bones[i+3]=0;
			skeleton->bones[i+4]=0;
			skeleton->bones[i+5]=1;
			skeleton->bones[i+6]=0;
			skeleton->bones[i+7]=0;
			skeleton->bones[i+8]=0;
			skeleton->bones[i+9]=0;
			skeleton->bones[i+10]=1;
			skeleton->bones[i+11]=0;
		}

	}



	if (p_bones) {
		glGenBuffers(1, &skeleton->ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, skeleton->ubo);
		glBufferData(GL_UNIFORM_BUFFER, skeleton->bones.size()*sizeof(float), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	if (!skeleton->update_list.in_list()) {
		skeleton_update_list.add(&skeleton->update_list);
	}



}
int RasterizerStorageGLES3::skeleton_get_bone_count(RID p_skeleton) const{

	Skeleton *skeleton = skeleton_owner.getornull(p_skeleton);
	ERR_FAIL_COND_V(!skeleton,0);

	return skeleton->size;
}

void RasterizerStorageGLES3::skeleton_bone_set_transform(RID p_skeleton,int p_bone, const Transform& p_transform){

	Skeleton *skeleton = skeleton_owner.getornull(p_skeleton);

	ERR_FAIL_COND(!skeleton);
	ERR_FAIL_INDEX(p_bone,skeleton->size);
	ERR_FAIL_COND(skeleton->use_2d);

	float * bones = skeleton->bones.ptr();
	bones[p_bone*12+ 0]=p_transform.basis.elements[0][0];
	bones[p_bone*12+ 1]=p_transform.basis.elements[0][1];
	bones[p_bone*12+ 2]=p_transform.basis.elements[0][2];
	bones[p_bone*12+ 3]=p_transform.origin.x;
	bones[p_bone*12+ 4]=p_transform.basis.elements[1][0];
	bones[p_bone*12+ 5]=p_transform.basis.elements[1][1];
	bones[p_bone*12+ 6]=p_transform.basis.elements[1][2];
	bones[p_bone*12+ 7]=p_transform.origin.y;
	bones[p_bone*12+ 8]=p_transform.basis.elements[2][0];
	bones[p_bone*12+ 9]=p_transform.basis.elements[2][1];
	bones[p_bone*12+10]=p_transform.basis.elements[2][2];
	bones[p_bone*12+11]=p_transform.origin.z;

	if (!skeleton->update_list.in_list()) {
		skeleton_update_list.add(&skeleton->update_list);
	}

}


Transform RasterizerStorageGLES3::skeleton_bone_get_transform(RID p_skeleton,int p_bone) const{

	Skeleton *skeleton = skeleton_owner.getornull(p_skeleton);

	ERR_FAIL_COND_V(!skeleton,Transform());
	ERR_FAIL_INDEX_V(p_bone,skeleton->size,Transform());
	ERR_FAIL_COND_V(skeleton->use_2d,Transform());

	float * bones = skeleton->bones.ptr();
	Transform mtx;
	mtx.basis.elements[0][0]=bones[p_bone*12+ 0];
	mtx.basis.elements[0][1]=bones[p_bone*12+ 1];
	mtx.basis.elements[0][2]=bones[p_bone*12+ 2];
	mtx.origin.x=bones[p_bone*12+ 3];
	mtx.basis.elements[1][0]=bones[p_bone*12+ 4];
	mtx.basis.elements[1][1]=bones[p_bone*12+ 5];
	mtx.basis.elements[1][2]=bones[p_bone*12+ 6];
	mtx.origin.y=bones[p_bone*12+ 7];
	mtx.basis.elements[2][0]=bones[p_bone*12+ 8];
	mtx.basis.elements[2][1]=bones[p_bone*12+ 9];
	mtx.basis.elements[2][2]=bones[p_bone*12+10];
	mtx.origin.z=bones[p_bone*12+11];

	return mtx;
}
void RasterizerStorageGLES3::skeleton_bone_set_transform_2d(RID p_skeleton,int p_bone, const Transform2D& p_transform){

	Skeleton *skeleton = skeleton_owner.getornull(p_skeleton);

	ERR_FAIL_COND(!skeleton);
	ERR_FAIL_INDEX(p_bone,skeleton->size);
	ERR_FAIL_COND(!skeleton->use_2d);

	float * bones = skeleton->bones.ptr();
	bones[p_bone*12+ 0]=p_transform.elements[0][0];
	bones[p_bone*12+ 1]=p_transform.elements[1][0];
	bones[p_bone*12+ 2]=0;
	bones[p_bone*12+ 3]=p_transform.elements[2][0];
	bones[p_bone*12+ 4]=p_transform.elements[0][1];
	bones[p_bone*12+ 5]=p_transform.elements[1][1];
	bones[p_bone*12+ 6]=0;
	bones[p_bone*12+ 7]=p_transform.elements[2][1];

	if (!skeleton->update_list.in_list()) {
		skeleton_update_list.add(&skeleton->update_list);
	}

}
Transform2D RasterizerStorageGLES3::skeleton_bone_get_transform_2d(RID p_skeleton,int p_bone) const{

	Skeleton *skeleton = skeleton_owner.getornull(p_skeleton);


	ERR_FAIL_COND_V(!skeleton,Transform2D());
	ERR_FAIL_INDEX_V(p_bone,skeleton->size,Transform2D());
	ERR_FAIL_COND_V(!skeleton->use_2d,Transform2D());

	Transform2D mtx;

	float * bones = skeleton->bones.ptr();
	mtx.elements[0][0]=bones[p_bone*12+ 0];
	mtx.elements[1][0]=bones[p_bone*12+ 1];
	mtx.elements[2][0]=bones[p_bone*12+ 3];
	mtx.elements[0][1]=bones[p_bone*12+ 4];
	mtx.elements[1][1]=bones[p_bone*12+ 5];
	mtx.elements[2][1]=bones[p_bone*12+ 7];

	return mtx;
}

void RasterizerStorageGLES3::update_dirty_skeletons() {

	while(skeleton_update_list.first()) {

		Skeleton *skeleton = skeleton_update_list.first()->self();
		if (skeleton->size) {
			glBindBuffer(GL_UNIFORM_BUFFER, skeleton->ubo);
			glBufferSubData(GL_UNIFORM_BUFFER,0,skeleton->bones.size()*sizeof(float),skeleton->bones.ptr());
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		for (Set<RasterizerScene::InstanceBase*>::Element *E=skeleton->instances.front();E;E=E->next()) {
			E->get()->base_changed();
		}

		skeleton_update_list.remove(skeleton_update_list.first());
	}

}

/* Light API */

RID RasterizerStorageGLES3::light_create(VS::LightType p_type){

	Light *light = memnew( Light );
	light->type=p_type;

	light->param[VS::LIGHT_PARAM_ENERGY]=1.0;
	light->param[VS::LIGHT_PARAM_SPECULAR]=0.5;
	light->param[VS::LIGHT_PARAM_RANGE]=1.0;
	light->param[VS::LIGHT_PARAM_SPOT_ANGLE]=45;
	light->param[VS::LIGHT_PARAM_SHADOW_MAX_DISTANCE]=0;
	light->param[VS::LIGHT_PARAM_SHADOW_SPLIT_1_OFFSET]=0.1;
	light->param[VS::LIGHT_PARAM_SHADOW_SPLIT_2_OFFSET]=0.3;
	light->param[VS::LIGHT_PARAM_SHADOW_SPLIT_3_OFFSET]=0.6;
	light->param[VS::LIGHT_PARAM_SHADOW_NORMAL_BIAS]=0.1;
	light->param[VS::LIGHT_PARAM_SHADOW_BIAS_SPLIT_SCALE]=0.1;


	light->color=Color(1,1,1,1);
	light->shadow=false;
	light->negative=false;
	light->cull_mask=0xFFFFFFFF;
	light->directional_shadow_mode=VS::LIGHT_DIRECTIONAL_SHADOW_ORTHOGONAL;
	light->omni_shadow_mode=VS::LIGHT_OMNI_SHADOW_DUAL_PARABOLOID;
	light->omni_shadow_detail=VS::LIGHT_OMNI_SHADOW_DETAIL_VERTICAL;
	light->directional_blend_splits=false;

	light->version=0;

	return light_owner.make_rid(light);
}

void RasterizerStorageGLES3::light_set_color(RID p_light,const Color& p_color){

	Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND(!light);

	light->color=p_color;
}
void RasterizerStorageGLES3::light_set_param(RID p_light,VS::LightParam p_param,float p_value){

	Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND(!light);
	ERR_FAIL_INDEX(p_param,VS::LIGHT_PARAM_MAX);

	switch(p_param) {
		case VS::LIGHT_PARAM_RANGE:
		case VS::LIGHT_PARAM_SPOT_ANGLE:
		case VS::LIGHT_PARAM_SHADOW_MAX_DISTANCE:
		case VS::LIGHT_PARAM_SHADOW_SPLIT_1_OFFSET:
		case VS::LIGHT_PARAM_SHADOW_SPLIT_2_OFFSET:
		case VS::LIGHT_PARAM_SHADOW_SPLIT_3_OFFSET:
		case VS::LIGHT_PARAM_SHADOW_NORMAL_BIAS:
		case VS::LIGHT_PARAM_SHADOW_BIAS:
		case VS::LIGHT_PARAM_SHADOW_BIAS_SPLIT_SCALE: {

			light->version++;
			light->instance_change_notify();
		} break;
	}

	light->param[p_param]=p_value;
}
void RasterizerStorageGLES3::light_set_shadow(RID p_light,bool p_enabled){

	Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND(!light);
	light->shadow=p_enabled;

	light->version++;
	light->instance_change_notify();
}

void RasterizerStorageGLES3::light_set_shadow_color(RID p_light,const Color& p_color) {

	Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND(!light);
	light->shadow_color=p_color;

}

void RasterizerStorageGLES3::light_set_projector(RID p_light,RID p_texture){

	Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND(!light);

	light->projector=p_texture;
}

void RasterizerStorageGLES3::light_set_negative(RID p_light,bool p_enable){

	Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND(!light);

	light->negative=p_enable;
}
void RasterizerStorageGLES3::light_set_cull_mask(RID p_light,uint32_t p_mask){

	Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND(!light);

	light->cull_mask=p_mask;

	light->version++;
	light->instance_change_notify();

}

void RasterizerStorageGLES3::light_omni_set_shadow_mode(RID p_light,VS::LightOmniShadowMode p_mode) {

	Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND(!light);

	light->omni_shadow_mode=p_mode;

	light->version++;
	light->instance_change_notify();


}

VS::LightOmniShadowMode RasterizerStorageGLES3::light_omni_get_shadow_mode(RID p_light) {

	const Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND_V(!light,VS::LIGHT_OMNI_SHADOW_CUBE);

	return light->omni_shadow_mode;
}


void RasterizerStorageGLES3::light_omni_set_shadow_detail(RID p_light,VS::LightOmniShadowDetail p_detail) {

	Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND(!light);

	light->omni_shadow_detail=p_detail;
	light->version++;
	light->instance_change_notify();
}


void RasterizerStorageGLES3::light_directional_set_shadow_mode(RID p_light,VS::LightDirectionalShadowMode p_mode){

	Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND(!light);

	light->directional_shadow_mode=p_mode;
	light->version++;
	light->instance_change_notify();

}

void RasterizerStorageGLES3::light_directional_set_blend_splits(RID p_light,bool p_enable) {

	Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND(!light);

	light->directional_blend_splits=p_enable;
	light->version++;
	light->instance_change_notify();

}


bool RasterizerStorageGLES3::light_directional_get_blend_splits(RID p_light) const {

	const Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND_V(!light,false);

	return light->directional_blend_splits;
}

VS::LightDirectionalShadowMode RasterizerStorageGLES3::light_directional_get_shadow_mode(RID p_light) {

	const Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND_V(!light,VS::LIGHT_DIRECTIONAL_SHADOW_ORTHOGONAL);

	return light->directional_shadow_mode;
}


VS::LightType RasterizerStorageGLES3::light_get_type(RID p_light) const {

	const Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND_V(!light,VS::LIGHT_DIRECTIONAL);

	return light->type;
}

float RasterizerStorageGLES3::light_get_param(RID p_light,VS::LightParam p_param) {

	const Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND_V(!light,VS::LIGHT_DIRECTIONAL);

	return light->param[p_param];
}

Color RasterizerStorageGLES3::light_get_color(RID p_light) {

	const Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND_V(!light,Color());

	return light->color;

}

bool RasterizerStorageGLES3::light_has_shadow(RID p_light) const {

	const Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND_V(!light,VS::LIGHT_DIRECTIONAL);

	return light->shadow;
}

uint64_t RasterizerStorageGLES3::light_get_version(RID p_light) const {

	const Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND_V(!light,0);

	return light->version;
}


Rect3 RasterizerStorageGLES3::light_get_aabb(RID p_light) const {

	const Light * light = light_owner.getornull(p_light);
	ERR_FAIL_COND_V(!light,Rect3());

	switch( light->type ) {

		case VS::LIGHT_SPOT: {

			float len=light->param[VS::LIGHT_PARAM_RANGE];
			float size=Math::tan(Math::deg2rad(light->param[VS::LIGHT_PARAM_SPOT_ANGLE]))*len;
			return Rect3( Vector3( -size,-size,-len ), Vector3( size*2, size*2, len ) );
		} break;
		case VS::LIGHT_OMNI: {

			float r = light->param[VS::LIGHT_PARAM_RANGE];
			return Rect3( -Vector3(r,r,r), Vector3(r,r,r)*2 );
		} break;
		case VS::LIGHT_DIRECTIONAL: {

			return Rect3();
		} break;
		default: {}
	}

	ERR_FAIL_V( Rect3() );
	return Rect3();
}

/* PROBE API */

RID RasterizerStorageGLES3::reflection_probe_create(){

	ReflectionProbe *reflection_probe = memnew( ReflectionProbe );

	reflection_probe->intensity=1.0;
	reflection_probe->interior_ambient=Color();
	reflection_probe->interior_ambient_energy=1.0;
	reflection_probe->max_distance=0;
	reflection_probe->extents=Vector3(1,1,1);
	reflection_probe->origin_offset=Vector3(0,0,0);
	reflection_probe->interior=false;
	reflection_probe->box_projection=false;
	reflection_probe->enable_shadows=false;
	reflection_probe->cull_mask=(1<<20)-1;
	reflection_probe->update_mode=VS::REFLECTION_PROBE_UPDATE_ONCE;

	return reflection_probe_owner.make_rid(reflection_probe);
}

void RasterizerStorageGLES3::reflection_probe_set_update_mode(RID p_probe, VS::ReflectionProbeUpdateMode p_mode) {

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->update_mode=p_mode;
	reflection_probe->instance_change_notify();

}

void RasterizerStorageGLES3::reflection_probe_set_intensity(RID p_probe, float p_intensity) {

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->intensity=p_intensity;

}

void RasterizerStorageGLES3::reflection_probe_set_interior_ambient(RID p_probe, const Color& p_ambient) {

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->interior_ambient=p_ambient;

}

void RasterizerStorageGLES3::reflection_probe_set_interior_ambient_energy(RID p_probe, float p_energy) {

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->interior_ambient_energy=p_energy;

}

void RasterizerStorageGLES3::reflection_probe_set_interior_ambient_probe_contribution(RID p_probe, float p_contrib) {

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->interior_ambient_probe_contrib=p_contrib;

}


void RasterizerStorageGLES3::reflection_probe_set_max_distance(RID p_probe, float p_distance){

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->max_distance=p_distance;
	reflection_probe->instance_change_notify();

}
void RasterizerStorageGLES3::reflection_probe_set_extents(RID p_probe, const Vector3& p_extents){

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->extents=p_extents;
	reflection_probe->instance_change_notify();

}
void RasterizerStorageGLES3::reflection_probe_set_origin_offset(RID p_probe, const Vector3& p_offset){

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->origin_offset=p_offset;
	reflection_probe->instance_change_notify();

}

void RasterizerStorageGLES3::reflection_probe_set_as_interior(RID p_probe, bool p_enable){

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->interior=p_enable;

}
void RasterizerStorageGLES3::reflection_probe_set_enable_box_projection(RID p_probe, bool p_enable){

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->box_projection=p_enable;

}

void RasterizerStorageGLES3::reflection_probe_set_enable_shadows(RID p_probe, bool p_enable){

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->enable_shadows=p_enable;
	reflection_probe->instance_change_notify();

}
void RasterizerStorageGLES3::reflection_probe_set_cull_mask(RID p_probe, uint32_t p_layers){

	ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!reflection_probe);

	reflection_probe->cull_mask=p_layers;
	reflection_probe->instance_change_notify();

}

Rect3 RasterizerStorageGLES3::reflection_probe_get_aabb(RID p_probe) const {
	const ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!reflection_probe,Rect3());

	Rect3 aabb;
	aabb.pos=-reflection_probe->extents;
	aabb.size=reflection_probe->extents*2.0;

	return aabb;


}
VS::ReflectionProbeUpdateMode RasterizerStorageGLES3::reflection_probe_get_update_mode(RID p_probe) const{

	const ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!reflection_probe,VS::REFLECTION_PROBE_UPDATE_ALWAYS);

	return reflection_probe->update_mode;
}

uint32_t RasterizerStorageGLES3::reflection_probe_get_cull_mask(RID p_probe) const {

	const ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!reflection_probe,0);

	return reflection_probe->cull_mask;

}

Vector3 RasterizerStorageGLES3::reflection_probe_get_extents(RID p_probe) const {

	const ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!reflection_probe,Vector3());

	return reflection_probe->extents;

}
Vector3 RasterizerStorageGLES3::reflection_probe_get_origin_offset(RID p_probe) const{

	const ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!reflection_probe,Vector3());

	return reflection_probe->origin_offset;

}

bool RasterizerStorageGLES3::reflection_probe_renders_shadows(RID p_probe) const {

	const ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!reflection_probe,false);

	return reflection_probe->enable_shadows;

}

float RasterizerStorageGLES3::reflection_probe_get_origin_max_distance(RID p_probe) const{

	const ReflectionProbe *reflection_probe = reflection_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!reflection_probe,0);

	return reflection_probe->max_distance;

}

/* ROOM API */

RID RasterizerStorageGLES3::room_create(){

	return RID();
}
void RasterizerStorageGLES3::room_add_bounds(RID p_room, const PoolVector<Vector2>& p_convex_polygon,float p_height,const Transform& p_transform){


}
void RasterizerStorageGLES3::room_clear_bounds(RID p_room){


}

/* PORTAL API */

// portals are only (x/y) points, forming a convex shape, which its clockwise
// order points outside. (z is 0);

RID RasterizerStorageGLES3::portal_create(){

	return RID();
}
void RasterizerStorageGLES3::portal_set_shape(RID p_portal, const Vector<Point2>& p_shape){


}
void RasterizerStorageGLES3::portal_set_enabled(RID p_portal, bool p_enabled){


}
void RasterizerStorageGLES3::portal_set_disable_distance(RID p_portal, float p_distance){


}
void RasterizerStorageGLES3::portal_set_disabled_color(RID p_portal, const Color& p_color){


}

RID RasterizerStorageGLES3::gi_probe_create() {

	GIProbe *gip = memnew( GIProbe );

	gip->bounds=Rect3(Vector3(),Vector3(1,1,1));
	gip->dynamic_range=1.0;
	gip->energy=1.0;
	gip->interior=false;
	gip->compress=false;
	gip->version=1;
	gip->cell_size=1.0;

	return gi_probe_owner.make_rid(gip);
}

void RasterizerStorageGLES3::gi_probe_set_bounds(RID p_probe,const Rect3& p_bounds){

	GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!gip);

	gip->bounds=p_bounds;
	gip->version++;
	gip->instance_change_notify();
}
Rect3 RasterizerStorageGLES3::gi_probe_get_bounds(RID p_probe) const{

	const GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!gip,Rect3());

	return gip->bounds;
}

void RasterizerStorageGLES3::gi_probe_set_cell_size(RID p_probe,float p_size) {

	GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!gip);

	gip->cell_size=p_size;
	gip->version++;
	gip->instance_change_notify();
}

float RasterizerStorageGLES3::gi_probe_get_cell_size(RID p_probe) const {

	const GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!gip,0);

	return gip->cell_size;

}

void RasterizerStorageGLES3::gi_probe_set_to_cell_xform(RID p_probe,const Transform& p_xform) {

	GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!gip);

	gip->to_cell=p_xform;
}

Transform RasterizerStorageGLES3::gi_probe_get_to_cell_xform(RID p_probe) const {

	const GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!gip,Transform());

	return gip->to_cell;

}



void RasterizerStorageGLES3::gi_probe_set_dynamic_data(RID p_probe,const PoolVector<int>& p_data){
	GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!gip);

	gip->dynamic_data=p_data;
	gip->version++;
	gip->instance_change_notify();

}
PoolVector<int> RasterizerStorageGLES3::gi_probe_get_dynamic_data(RID p_probe) const{

	const GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!gip,PoolVector<int>());

	return gip->dynamic_data;
}

void RasterizerStorageGLES3::gi_probe_set_dynamic_range(RID p_probe,int p_range){

	GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!gip);

	gip->dynamic_range=p_range;

}
int RasterizerStorageGLES3::gi_probe_get_dynamic_range(RID p_probe) const{

	const GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!gip,0);

	return gip->dynamic_range;
}

void RasterizerStorageGLES3::gi_probe_set_energy(RID p_probe,float p_range){

	GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!gip);

	gip->energy=p_range;

}

void RasterizerStorageGLES3::gi_probe_set_interior(RID p_probe,bool p_enable) {

	GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!gip);

	gip->interior=p_enable;

}

bool RasterizerStorageGLES3::gi_probe_is_interior(RID p_probe) const{

	const GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!gip,false);

	return gip->interior;

}


void RasterizerStorageGLES3::gi_probe_set_compress(RID p_probe,bool p_enable) {

	GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND(!gip);

	gip->compress=p_enable;

}

bool RasterizerStorageGLES3::gi_probe_is_compressed(RID p_probe) const{

	const GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!gip,false);

	return gip->compress;

}
float RasterizerStorageGLES3::gi_probe_get_energy(RID p_probe) const{

	const GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!gip,0);

	return gip->energy;
}



uint32_t RasterizerStorageGLES3::gi_probe_get_version(RID p_probe) {

	const GIProbe *gip = gi_probe_owner.getornull(p_probe);
	ERR_FAIL_COND_V(!gip,0);

	return gip->version;
}

RasterizerStorage::GIProbeCompression RasterizerStorageGLES3::gi_probe_get_dynamic_data_get_preferred_compression() const {
	if (config.s3tc_supported) {
		return GI_PROBE_S3TC;
	} else {
		return GI_PROBE_UNCOMPRESSED;
	}
}

RID RasterizerStorageGLES3::gi_probe_dynamic_data_create(int p_width, int p_height, int p_depth, GIProbeCompression p_compression) {

	GIProbeData *gipd = memnew( GIProbeData );

	gipd->width=p_width;
	gipd->height=p_height;
	gipd->depth=p_depth;
	gipd->compression=p_compression;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&gipd->tex_id);
	glBindTexture(GL_TEXTURE_3D,gipd->tex_id);

	int level=0;
	int min_size=1;

	if (gipd->compression==GI_PROBE_S3TC) {
		min_size=4;
	}

	print_line("dyndata create");
	while(true) {

		if (gipd->compression==GI_PROBE_S3TC) {
			int size = p_width * p_height * p_depth;
			glCompressedTexImage3D(GL_TEXTURE_3D,level,_EXT_COMPRESSED_RGBA_S3TC_DXT5_EXT,p_width,p_height,p_depth,0, size,NULL);
		} else {
			glTexImage3D(GL_TEXTURE_3D,level,GL_RGBA8,p_width,p_height,p_depth,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
		}

		if (p_width<=min_size || p_height<=min_size || p_depth<=min_size)
			break;
		p_width>>=1;
		p_height>>=1;
		p_depth>>=1;
		level++;
	}

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, level);

	gipd->levels=level+1;

	return gi_probe_data_owner.make_rid(gipd);
}

void RasterizerStorageGLES3::gi_probe_dynamic_data_update(RID p_gi_probe_data, int p_depth_slice, int p_slice_count, int p_mipmap, const void *p_data) {

	GIProbeData *gipd = gi_probe_data_owner.getornull(p_gi_probe_data);
	ERR_FAIL_COND(!gipd);
/*
	Vector<uint8_t> data;
	data.resize((gipd->width>>p_mipmap)*(gipd->height>>p_mipmap)*(gipd->depth>>p_mipmap)*4);

	for(int i=0;i<(gipd->width>>p_mipmap);i++) {
		for(int j=0;j<(gipd->height>>p_mipmap);j++) {
			for(int k=0;k<(gipd->depth>>p_mipmap);k++) {

				int ofs = (k*(gipd->height>>p_mipmap)*(gipd->width>>p_mipmap)) + j *(gipd->width>>p_mipmap) + i;
				ofs*=4;
				data[ofs+0]=i*0xFF/(gipd->width>>p_mipmap);
				data[ofs+1]=j*0xFF/(gipd->height>>p_mipmap);
				data[ofs+2]=k*0xFF/(gipd->depth>>p_mipmap);
				data[ofs+3]=0xFF;
			}
		}
	}
*/
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D,gipd->tex_id);
	if (gipd->compression==GI_PROBE_S3TC) {
		int size = (gipd->width>>p_mipmap) * (gipd->height>>p_mipmap) * p_slice_count;
		glCompressedTexSubImage3D(GL_TEXTURE_3D,p_mipmap,0,0,p_depth_slice,gipd->width>>p_mipmap,gipd->height>>p_mipmap,p_slice_count,_EXT_COMPRESSED_RGBA_S3TC_DXT5_EXT,size, p_data);
	} else {
		glTexSubImage3D(GL_TEXTURE_3D,p_mipmap,0,0,p_depth_slice,gipd->width>>p_mipmap,gipd->height>>p_mipmap,p_slice_count,GL_RGBA,GL_UNSIGNED_BYTE,p_data);
	}
	//glTexImage3D(GL_TEXTURE_3D,p_mipmap,GL_RGBA8,gipd->width>>p_mipmap,gipd->height>>p_mipmap,gipd->depth>>p_mipmap,0,GL_RGBA,GL_UNSIGNED_BYTE,p_data);
	//glTexImage3D(GL_TEXTURE_3D,p_mipmap,GL_RGBA8,gipd->width>>p_mipmap,gipd->height>>p_mipmap,gipd->depth>>p_mipmap,0,GL_RGBA,GL_UNSIGNED_BYTE,data.ptr());

}

///////



RID RasterizerStorageGLES3::particles_create() {

	Particles *particles = memnew( Particles );


	return particles_owner.make_rid(particles);
}

void RasterizerStorageGLES3::particles_set_emitting(RID p_particles,bool p_emitting) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);
	particles->emitting=p_emitting;

}
void RasterizerStorageGLES3::particles_set_amount(RID p_particles,int p_amount) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);

	int floats = p_amount*24;
	float * data = memnew_arr(float,floats);

	for(int i=0;i<floats;i++) {
		data[i]=0;
	}


	glBindBuffer(GL_ARRAY_BUFFER,particles->particle_buffers[0]);
	glBufferData(GL_ARRAY_BUFFER,floats*sizeof(float),data,GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER,particles->particle_buffers[1]);
	glBufferData(GL_ARRAY_BUFFER,floats*sizeof(float),data,GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER,0);

	particles->prev_ticks=0;
	particles->phase=0;
	particles->prev_phase=0;

	memdelete_arr(data);

}

void RasterizerStorageGLES3::particles_set_lifetime(RID p_particles,float p_lifetime){

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);
	particles->lifetime=p_lifetime;
}
void RasterizerStorageGLES3::particles_set_pre_process_time(RID p_particles,float p_time) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);
	particles->pre_process_time=p_time;

}
void RasterizerStorageGLES3::particles_set_explosiveness_ratio(RID p_particles,float p_ratio) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);
	particles->explosiveness=p_ratio;
}
void RasterizerStorageGLES3::particles_set_randomness_ratio(RID p_particles,float p_ratio) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);
	particles->randomness=p_ratio;

}
void RasterizerStorageGLES3::particles_set_custom_aabb(RID p_particles,const Rect3& p_aabb) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);
	particles->custom_aabb=p_aabb;

}
void RasterizerStorageGLES3::particles_set_gravity(RID p_particles,const Vector3& p_gravity) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);

	particles->gravity=p_gravity;

}
void RasterizerStorageGLES3::particles_set_use_local_coordinates(RID p_particles,bool p_enable) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);

	particles->use_local_coords=p_enable;
}
void RasterizerStorageGLES3::particles_set_process_material(RID p_particles,RID p_material) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);

	particles->process_material=p_material;
}

void RasterizerStorageGLES3::particles_set_emission_shape(RID p_particles, VS::ParticlesEmissionShape p_shape) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);

	particles->emission_shape=p_shape;
}
void RasterizerStorageGLES3::particles_set_emission_sphere_radius(RID p_particles,float p_radius) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);

	particles->emission_sphere_radius=p_radius;
}
void RasterizerStorageGLES3::particles_set_emission_box_extents(RID p_particles,const Vector3& p_extents) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);

	particles->emission_box_extents=p_extents;
}
void RasterizerStorageGLES3::particles_set_emission_points(RID p_particles,const PoolVector<Vector3>& p_points) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);

	particles->emission_points=p_points;
}


void RasterizerStorageGLES3::particles_set_draw_order(RID p_particles,VS::ParticlesDrawOrder p_order) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);

	particles->draw_order=p_order;
}

void RasterizerStorageGLES3::particles_set_draw_passes(RID p_particles,int p_count) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);

	particles->draw_passes.resize(p_count);
}
void RasterizerStorageGLES3::particles_set_draw_pass_material(RID p_particles,int p_pass, RID p_material) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);
	ERR_FAIL_INDEX(p_pass,particles->draw_passes.size());
	particles->draw_passes[p_pass].material=p_material;

}
void RasterizerStorageGLES3::particles_set_draw_pass_mesh(RID p_particles,int p_pass, RID p_mesh) {

	Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND(!particles);
	ERR_FAIL_INDEX(p_pass,particles->draw_passes.size());
	particles->draw_passes[p_pass].mesh=p_mesh;

}

Rect3 RasterizerStorageGLES3::particles_get_current_aabb(RID p_particles) {

	const Particles *particles = particles_owner.getornull(p_particles);
	ERR_FAIL_COND_V(!particles,Rect3());

	return particles->computed_aabb;
}

void RasterizerStorageGLES3::update_particles() {

	glEnable(GL_RASTERIZER_DISCARD);
	glBindVertexArray(0);


	while (particle_update_list.first()) {

		//use transform feedback to process particles

		Particles *particles = particle_update_list.first()->self();


		Material *material = material_owner.getornull(particles->process_material);
		if (!material || !material->shader || material->shader->mode!=VS::SHADER_PARTICLES) {

			shaders.particles.set_custom_shader(0);
		} else {
			shaders.particles.set_custom_shader( material->shader->custom_code_id );

			if (material->ubo_id) {

				glBindBufferBase(GL_UNIFORM_BUFFER,0,material->ubo_id);
			}

			int tc = material->textures.size();
			RID* textures = material->textures.ptr();
			ShaderLanguage::ShaderNode::Uniform::Hint* texture_hints = material->shader->texture_hints.ptr();


			for(int i=0;i<tc;i++) {

				glActiveTexture(GL_TEXTURE0+i);

				GLenum target;
				GLuint tex;

				RasterizerStorageGLES3::Texture *t = texture_owner.getornull( textures[i] );

				if (!t) {
					//check hints
					target=GL_TEXTURE_2D;

					switch(texture_hints[i]) {
						case ShaderLanguage::ShaderNode::Uniform::HINT_BLACK_ALBEDO:
						case ShaderLanguage::ShaderNode::Uniform::HINT_BLACK: {
							tex=resources.black_tex;
						} break;
						case ShaderLanguage::ShaderNode::Uniform::HINT_ANISO: {
							tex=resources.aniso_tex;
						} break;
						case ShaderLanguage::ShaderNode::Uniform::HINT_NORMAL: {
							tex=resources.normal_tex;
						} break;
						default: {
							tex=resources.white_tex;
						} break;
					}


				} else {

					target=t->target;
					tex = t->tex_id;

				}

				glBindTexture(target,tex);
			}

		}

		shaders.particles.bind();

		shaders.particles.set_uniform(ParticlesShaderGLES3::ORIGIN,particles->origin);

		float new_phase = Math::fmod(particles->phase+(frame.delta/particles->lifetime),1.0);

		shaders.particles.set_uniform(ParticlesShaderGLES3::SYSTEM_PHASE,new_phase);
		shaders.particles.set_uniform(ParticlesShaderGLES3::PREV_SYSTEM_PHASE,particles->phase);
		particles->phase = new_phase;

		shaders.particles.set_uniform(ParticlesShaderGLES3::TOTAL_PARTICLES,particles->amount);
		shaders.particles.set_uniform(ParticlesShaderGLES3::TIME,0.0);
		shaders.particles.set_uniform(ParticlesShaderGLES3::EXPLOSIVENESS,particles->explosiveness);
		shaders.particles.set_uniform(ParticlesShaderGLES3::DELTA,frame.delta);
		shaders.particles.set_uniform(ParticlesShaderGLES3::GRAVITY,particles->gravity);
		shaders.particles.set_uniform(ParticlesShaderGLES3::ATTRACTOR_COUNT,0);




		glBindBuffer(GL_ARRAY_BUFFER,particles->particle_buffers[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particles->particle_buffers[1]);

		for(int i=0;i<6;i++) {
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i,4,GL_FLOAT,GL_FALSE,sizeof(float)*4*6,((uint8_t*)0)+(i*16));
		}


		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS,0,particles->amount);
		glEndTransformFeedback();

		particle_update_list.remove(particle_update_list.first());

		SWAP(particles->particle_buffers[0],particles->particle_buffers[1]);
	}

	glDisable(GL_RASTERIZER_DISCARD);

	for(int i=0;i<6;i++) {
		glDisableVertexAttribArray(i);
	}

}

////////

void RasterizerStorageGLES3::instance_add_skeleton(RID p_skeleton,RasterizerScene::InstanceBase *p_instance) {

	Skeleton *skeleton = skeleton_owner.getornull(p_skeleton);
	ERR_FAIL_COND(!skeleton);

	skeleton->instances.insert(p_instance);
}

void RasterizerStorageGLES3::instance_remove_skeleton(RID p_skeleton,RasterizerScene::InstanceBase *p_instance) {

	Skeleton *skeleton = skeleton_owner.getornull(p_skeleton);
	ERR_FAIL_COND(!skeleton);

	skeleton->instances.erase(p_instance);
}


void RasterizerStorageGLES3::instance_add_dependency(RID p_base,RasterizerScene::InstanceBase *p_instance) {

	Instantiable *inst=NULL;
	switch(p_instance->base_type) {
		case VS::INSTANCE_MESH: {
			inst = mesh_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		case VS::INSTANCE_MULTIMESH: {
			inst = multimesh_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		case VS::INSTANCE_IMMEDIATE: {
			inst = immediate_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		case VS::INSTANCE_REFLECTION_PROBE: {
			inst = reflection_probe_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		case VS::INSTANCE_LIGHT: {
			inst = light_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		case VS::INSTANCE_GI_PROBE: {
			inst = gi_probe_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		default: {
			if (!inst) {
				ERR_FAIL();
			}
		}
	}

	inst->instance_list.add( &p_instance->dependency_item );
}

void RasterizerStorageGLES3::instance_remove_dependency(RID p_base,RasterizerScene::InstanceBase *p_instance){

	Instantiable *inst=NULL;

	switch(p_instance->base_type) {
		case VS::INSTANCE_MESH: {
			inst = mesh_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		case VS::INSTANCE_MULTIMESH: {
			inst = multimesh_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		case VS::INSTANCE_IMMEDIATE: {
			inst = immediate_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		case VS::INSTANCE_REFLECTION_PROBE: {
			inst = reflection_probe_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		case VS::INSTANCE_LIGHT: {
			inst = light_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		case VS::INSTANCE_GI_PROBE: {
			inst = gi_probe_owner.getornull(p_base);
			ERR_FAIL_COND(!inst);
		} break;
		default: {

			if (!inst) {
				ERR_FAIL();
			}
		}
	}

	ERR_FAIL_COND(!inst);

	inst->instance_list.remove( &p_instance->dependency_item );
}


/* RENDER TARGET */


void RasterizerStorageGLES3::_render_target_clear(RenderTarget *rt) {

	if (rt->fbo) {
		glDeleteFramebuffers(1,&rt->fbo);
		glDeleteTextures(1,&rt->color);
		rt->fbo=0;
	}

	if (rt->buffers.fbo) {
		glDeleteFramebuffers(1,&rt->buffers.fbo);
		glDeleteRenderbuffers(1,&rt->buffers.depth);
		glDeleteRenderbuffers(1,&rt->buffers.diffuse);
		glDeleteRenderbuffers(1,&rt->buffers.specular);
		glDeleteRenderbuffers(1,&rt->buffers.normal_rough);
		glDeleteRenderbuffers(1,&rt->buffers.motion_sss);
		glDeleteFramebuffers(1,&rt->buffers.effect_fbo);
		glDeleteTextures(1,&rt->buffers.effect);

		rt->buffers.fbo=0;
	}

	if (rt->depth) {
		glDeleteTextures(1,&rt->depth);
		rt->depth=0;
	}

	if (rt->effects.ssao.blur_fbo[0]) {
		glDeleteFramebuffers(1,&rt->effects.ssao.blur_fbo[0]);
		glDeleteTextures(1,&rt->effects.ssao.blur_red[0]);
		glDeleteFramebuffers(1,&rt->effects.ssao.blur_fbo[1]);
		glDeleteTextures(1,&rt->effects.ssao.blur_red[1]);
		for(int i=0;i<rt->effects.ssao.depth_mipmap_fbos.size();i++) {
			glDeleteFramebuffers(1,&rt->effects.ssao.depth_mipmap_fbos[i]);
		}

		rt->effects.ssao.depth_mipmap_fbos.clear();

		glDeleteTextures(1,&rt->effects.ssao.linear_depth);
	}

	if (rt->exposure.fbo) {
		glDeleteFramebuffers(1,&rt->exposure.fbo);
		glDeleteTextures(1,&rt->exposure.color);
	}
	Texture *tex = texture_owner.get(rt->texture);
	tex->alloc_height=0;
	tex->alloc_width=0;
	tex->width=0;
	tex->height=0;

	for(int i=0;i<2;i++) {
		for(int j=0;j<rt->effects.mip_maps[i].sizes.size();j++) {
			glDeleteFramebuffers(1,&rt->effects.mip_maps[i].sizes[j].fbo);
		}

		glDeleteTextures(1,&rt->effects.mip_maps[i].color);
		rt->effects.mip_maps[i].sizes.clear();
		rt->effects.mip_maps[i].levels=0;
	}
/*
	if (rt->effects.screen_space_depth) {
		glDeleteTextures(1,&rt->effects.screen_space_depth);
		rt->effects.screen_space_depth=0;

	}
*/
}

void RasterizerStorageGLES3::_render_target_allocate(RenderTarget *rt){

	if (rt->width<=0 || rt->height<=0)
		return;


	GLuint color_internal_format;
	GLuint color_format;
	GLuint color_type;
	Image::Format image_format;



	if (!rt->flags[RENDER_TARGET_HDR] || rt->flags[RENDER_TARGET_NO_3D]) {

		color_internal_format=GL_RGBA8;
		color_format=GL_RGBA;
		color_type=GL_UNSIGNED_BYTE;
		image_format=Image::FORMAT_RGBA8;
	} else {
		color_internal_format=GL_RGBA16F;
		color_format=GL_RGBA;
		color_type=GL_HALF_FLOAT;
		image_format=Image::FORMAT_RGBAH;
	}


	{
		/* FRONT FBO */

		glActiveTexture(GL_TEXTURE0);

		glGenFramebuffers(1, &rt->fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, rt->fbo);


		glGenTextures(1, &rt->depth);
		glBindTexture(GL_TEXTURE_2D, rt->depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, rt->width, rt->height, 0,
			     GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				       GL_TEXTURE_2D, rt->depth, 0);

		glGenTextures(1, &rt->color);
		glBindTexture(GL_TEXTURE_2D, rt->color);

		glTexImage2D(GL_TEXTURE_2D, 0, color_internal_format,  rt->width, rt->height, 0, color_format, color_type, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->color, 0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		glBindFramebuffer(GL_FRAMEBUFFER, config.system_fbo);

		ERR_FAIL_COND( status != GL_FRAMEBUFFER_COMPLETE );

		Texture *tex = texture_owner.get(rt->texture);
		tex->format=image_format;
		tex->gl_format_cache=color_format;
		tex->gl_type_cache=color_type;
		tex->gl_internal_format_cache=color_internal_format;
		tex->tex_id=rt->color;
		tex->width=rt->width;
		tex->alloc_width=rt->width;
		tex->height=rt->height;
		tex->alloc_height=rt->height;


		texture_set_flags(rt->texture,tex->flags);

	}


	/* BACK FBO */

	if (config.render_arch==RENDER_ARCH_DESKTOP && !rt->flags[RENDER_TARGET_NO_3D]) {



		static const int msaa_value[]={0,2,4,8,16};
		int msaa=msaa_value[rt->msaa];

		//regular fbo
		glGenFramebuffers(1, &rt->buffers.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, rt->buffers.fbo);

		glGenRenderbuffers(1, &rt->buffers.depth);
		glBindRenderbuffer(GL_RENDERBUFFER, rt->buffers.depth);
		if (msaa==0)
			glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,rt->width,rt->height);
		else
			glRenderbufferStorageMultisample(GL_RENDERBUFFER,msaa,GL_DEPTH24_STENCIL8,rt->width,rt->height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rt->buffers.depth);
	
		glGenRenderbuffers(1, &rt->buffers.diffuse);
		glBindRenderbuffer(GL_RENDERBUFFER, rt->buffers.diffuse);

		if (msaa==0)
			glRenderbufferStorage(GL_RENDERBUFFER,color_internal_format,rt->width,rt->height);
		else
			glRenderbufferStorageMultisample(GL_RENDERBUFFER,msaa,GL_RGBA16F,rt->width,rt->height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,rt->buffers.diffuse);

		glGenRenderbuffers(1, &rt->buffers.specular);
		glBindRenderbuffer(GL_RENDERBUFFER, rt->buffers.specular);

		if (msaa==0)
			glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA16F,rt->width,rt->height);
		else
			glRenderbufferStorageMultisample(GL_RENDERBUFFER,msaa,color_internal_format,rt->width,rt->height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_RENDERBUFFER,rt->buffers.specular);

		glGenRenderbuffers(1, &rt->buffers.normal_rough);
		glBindRenderbuffer(GL_RENDERBUFFER, rt->buffers.normal_rough);

		if (msaa==0)
			glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA8,rt->width,rt->height);
		else
			glRenderbufferStorageMultisample(GL_RENDERBUFFER,msaa,GL_RGBA8,rt->width,rt->height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2,GL_RENDERBUFFER,rt->buffers.normal_rough);


		glGenRenderbuffers(1, &rt->buffers.motion_sss);
		glBindRenderbuffer(GL_RENDERBUFFER, rt->buffers.motion_sss);

		if (msaa==0)
			glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA8,rt->width,rt->height);
		else
			glRenderbufferStorageMultisample(GL_RENDERBUFFER,msaa,GL_RGBA8,rt->width,rt->height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT3,GL_RENDERBUFFER,rt->buffers.motion_sss);



		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		glBindFramebuffer(GL_FRAMEBUFFER, config.system_fbo);

		if (status != GL_FRAMEBUFFER_COMPLETE) {
			printf("err status: %x\n",status);
			_render_target_clear(rt);
			ERR_FAIL_COND( status != GL_FRAMEBUFFER_COMPLETE );
		}		

		glBindRenderbuffer(GL_RENDERBUFFER,0);

		// effect resolver

		glGenFramebuffers(1, &rt->buffers.effect_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, rt->buffers.effect_fbo);

		glGenTextures(1, &rt->buffers.effect);
		glBindTexture(GL_TEXTURE_2D, rt->buffers.effect);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rt->width, rt->height, 0,
			     GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				       GL_TEXTURE_2D, rt->buffers.effect, 0);


		if (status != GL_FRAMEBUFFER_COMPLETE) {
			printf("err status: %x\n",status);
			_render_target_clear(rt);
			ERR_FAIL_COND( status != GL_FRAMEBUFFER_COMPLETE );
		}

		glBindFramebuffer(GL_FRAMEBUFFER, config.system_fbo);

		if (status != GL_FRAMEBUFFER_COMPLETE) {
			_render_target_clear(rt);
			ERR_FAIL_COND( status != GL_FRAMEBUFFER_COMPLETE );
		}

		for(int i=0;i<2;i++) {

			ERR_FAIL_COND( rt->effects.mip_maps[i].sizes.size() );
			int w=rt->width;
			int h=rt->height;


			if (i>0) {
				w>>=1;
				h>>=1;
			}


			glGenTextures(1, &rt->effects.mip_maps[i].color);
			glBindTexture(GL_TEXTURE_2D, rt->effects.mip_maps[i].color);

			int level=0;

			while(true) {

				RenderTarget::Effects::MipMaps::Size mm;

				glTexImage2D(GL_TEXTURE_2D, level, color_internal_format,  w, h, 0, color_format, color_type, NULL);
				mm.width=w;
				mm.height=h;
				rt->effects.mip_maps[i].sizes.push_back(mm);

				w>>=1;
				h>>=1;

				if (w<2 || h<2)
					break;

				level++;

			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);


			for(int j=0;j<rt->effects.mip_maps[i].sizes.size();j++) {

				RenderTarget::Effects::MipMaps::Size &mm=rt->effects.mip_maps[i].sizes[j];

				glGenFramebuffers(1, &mm.fbo);
				glBindFramebuffer(GL_FRAMEBUFFER, mm.fbo);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,rt->effects.mip_maps[i].color ,j);

				status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if (status != GL_FRAMEBUFFER_COMPLETE) {
					_render_target_clear(rt);
					ERR_FAIL_COND( status != GL_FRAMEBUFFER_COMPLETE );
				}


				float zero[4]={1,0,1,0};
				glClearBufferfv(GL_COLOR,0,zero);


			}

			glBindFramebuffer(GL_FRAMEBUFFER, config.system_fbo);
			rt->effects.mip_maps[i].levels=level;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		}
		///////////////// ssao

		//AO strength textures
		for(int i=0;i<2;i++) {

			glGenFramebuffers(1, &rt->effects.ssao.blur_fbo[i]);
			glBindFramebuffer(GL_FRAMEBUFFER, rt->effects.ssao.blur_fbo[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
					       GL_TEXTURE_2D, rt->depth, 0);

			glGenTextures(1, &rt->effects.ssao.blur_red[i]);
			glBindTexture(GL_TEXTURE_2D, rt->effects.ssao.blur_red[i]);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,  rt->width, rt->height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->effects.ssao.blur_red[i], 0);

			status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				_render_target_clear(rt);
				ERR_FAIL_COND( status != GL_FRAMEBUFFER_COMPLETE );
			}

		}
		//5 mip levels for depth texture, but base is read separately

		glGenTextures(1, &rt->effects.ssao.linear_depth);
		glBindTexture(GL_TEXTURE_2D, rt->effects.ssao.linear_depth);

		int ssao_w=rt->width/2;
		int ssao_h=rt->height/2;


		for(int i=0;i<4;i++) { //5, but 4 mips, base is read directly to save bw

			glTexImage2D(GL_TEXTURE_2D, i, GL_R16UI,  ssao_w, ssao_h, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, NULL);
			ssao_w>>=1;
			ssao_h>>=1;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);

		for(int i=0;i<4;i++) { //5, but 4 mips, base is read directly to save bw

			GLuint fbo;
			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->effects.ssao.linear_depth, i);
			rt->effects.ssao.depth_mipmap_fbos.push_back(fbo);
		}


		//////Exposure

		glGenFramebuffers(1, &rt->exposure.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, rt->exposure.fbo);

		glGenTextures(1, &rt->exposure.color);
		glBindTexture(GL_TEXTURE_2D, rt->exposure.color);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,  1, 1, 0, GL_RED, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  rt->exposure.color, 0);

		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			_render_target_clear(rt);
			ERR_FAIL_COND( status != GL_FRAMEBUFFER_COMPLETE );
		}

	}
}


RID RasterizerStorageGLES3::render_target_create(){

	RenderTarget *rt = memnew( RenderTarget );

	Texture * t = memnew( Texture );

	t->flags=0;
	t->width=0;
	t->height=0;
	t->alloc_height=0;
	t->alloc_width=0;
	t->format=Image::FORMAT_R8;
	t->target=GL_TEXTURE_2D;
	t->gl_format_cache=0;
	t->gl_internal_format_cache=0;
	t->gl_type_cache=0;
	t->data_size=0;
	t->compressed=false;
	t->srgb=false;
	t->total_data_size=0;
	t->ignore_mipmaps=false;
	t->mipmaps=0;
	t->active=true;
	t->tex_id=0;


	rt->texture=texture_owner.make_rid(t);

	return render_target_owner.make_rid(rt);
}

void RasterizerStorageGLES3::render_target_set_size(RID p_render_target,int p_width, int p_height){

	RenderTarget *rt = render_target_owner.getornull(p_render_target);
	ERR_FAIL_COND(!rt);

	if (rt->width==p_width && rt->height==p_height)
		return;

	_render_target_clear(rt);
	rt->width=p_width;
	rt->height=p_height;
	_render_target_allocate(rt);

}


RID RasterizerStorageGLES3::render_target_get_texture(RID p_render_target) const{

	RenderTarget *rt = render_target_owner.getornull(p_render_target);
	ERR_FAIL_COND_V(!rt,RID());

	return rt->texture;
}

void RasterizerStorageGLES3::render_target_set_flag(RID p_render_target,RenderTargetFlags p_flag,bool p_value) {

	RenderTarget *rt = render_target_owner.getornull(p_render_target);
	ERR_FAIL_COND(!rt);

	rt->flags[p_flag]=p_value;

	switch(p_flag) {
		case RENDER_TARGET_NO_3D:
		case RENDER_TARGET_TRANSPARENT: {
			//must reset for these formats
			_render_target_clear(rt);
			_render_target_allocate(rt);

		} break;
		default: {}
	}
}

bool RasterizerStorageGLES3::render_target_renedered_in_frame(RID p_render_target){

	return false;
}

void RasterizerStorageGLES3::render_target_set_msaa(RID p_render_target,VS::ViewportMSAA p_msaa) {

	RenderTarget *rt = render_target_owner.getornull(p_render_target);
	ERR_FAIL_COND(!rt);

	if (rt->msaa==p_msaa)
		return;

	_render_target_clear(rt);
	rt->msaa=p_msaa;
	_render_target_allocate(rt);

}

/* CANVAS SHADOW */


RID RasterizerStorageGLES3::canvas_light_shadow_buffer_create(int p_width) {

	CanvasLightShadow *cls = memnew( CanvasLightShadow );
	if (p_width>config.max_texture_size)
		p_width=config.max_texture_size;

	cls->size=p_width;
	cls->height=16;

	glActiveTexture(GL_TEXTURE0);

	glGenFramebuffers(1, &cls->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, cls->fbo);

	glGenRenderbuffers(1, &cls->depth);
	glBindRenderbuffer(GL_RENDERBUFFER, cls->depth );
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24, cls->size, cls->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cls->depth);
	glBindRenderbuffer(GL_RENDERBUFFER, 0 );

	glGenTextures(1,&cls->distance);
	glBindTexture(GL_TEXTURE_2D, cls->distance);
	if (config.use_rgba_2d_shadows) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, cls->size, cls->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, cls->size, cls->height, 0, GL_RED, GL_FLOAT, NULL);
	}




	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cls->distance, 0);


	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//printf("errnum: %x\n",status);
	glBindFramebuffer(GL_FRAMEBUFFER, config.system_fbo);

	ERR_FAIL_COND_V( status != GL_FRAMEBUFFER_COMPLETE, RID() );

	return canvas_light_shadow_owner.make_rid(cls);
}

/* LIGHT SHADOW MAPPING */


RID RasterizerStorageGLES3::canvas_light_occluder_create() {

	CanvasOccluder *co = memnew( CanvasOccluder );
	co->index_id=0;
	co->vertex_id=0;
	co->len=0;

	return canvas_occluder_owner.make_rid(co);
}

void RasterizerStorageGLES3::canvas_light_occluder_set_polylines(RID p_occluder, const PoolVector<Vector2>& p_lines) {

	CanvasOccluder *co = canvas_occluder_owner.get(p_occluder);
	ERR_FAIL_COND(!co);

	co->lines=p_lines;

	if (p_lines.size()!=co->len) {

		if (co->index_id)
			glDeleteBuffers(1,&co->index_id);
		if (co->vertex_id)
			glDeleteBuffers(1,&co->vertex_id);

		co->index_id=0;
		co->vertex_id=0;
		co->len=0;

	}

	if (p_lines.size()) {



		PoolVector<float> geometry;
		PoolVector<uint16_t> indices;
		int lc = p_lines.size();

		geometry.resize(lc*6);
		indices.resize(lc*3);

		PoolVector<float>::Write vw=geometry.write();
		PoolVector<uint16_t>::Write iw=indices.write();


		PoolVector<Vector2>::Read lr=p_lines.read();

		const int POLY_HEIGHT = 16384;

		for(int i=0;i<lc/2;i++) {

			vw[i*12+0]=lr[i*2+0].x;
			vw[i*12+1]=lr[i*2+0].y;
			vw[i*12+2]=POLY_HEIGHT;

			vw[i*12+3]=lr[i*2+1].x;
			vw[i*12+4]=lr[i*2+1].y;
			vw[i*12+5]=POLY_HEIGHT;

			vw[i*12+6]=lr[i*2+1].x;
			vw[i*12+7]=lr[i*2+1].y;
			vw[i*12+8]=-POLY_HEIGHT;

			vw[i*12+9]=lr[i*2+0].x;
			vw[i*12+10]=lr[i*2+0].y;
			vw[i*12+11]=-POLY_HEIGHT;

			iw[i*6+0]=i*4+0;
			iw[i*6+1]=i*4+1;
			iw[i*6+2]=i*4+2;

			iw[i*6+3]=i*4+2;
			iw[i*6+4]=i*4+3;
			iw[i*6+5]=i*4+0;

		}

		//if same buffer len is being set, just use BufferSubData to avoid a pipeline flush


		if (!co->vertex_id) {
			glGenBuffers(1,&co->vertex_id);
			glBindBuffer(GL_ARRAY_BUFFER,co->vertex_id);
			glBufferData(GL_ARRAY_BUFFER,lc*6*sizeof(real_t),vw.ptr(),GL_STATIC_DRAW);
		} else {

			glBindBuffer(GL_ARRAY_BUFFER,co->vertex_id);
			glBufferSubData(GL_ARRAY_BUFFER,0,lc*6*sizeof(real_t),vw.ptr());

		}

		glBindBuffer(GL_ARRAY_BUFFER,0); //unbind

		if (!co->index_id) {

			glGenBuffers(1,&co->index_id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,co->index_id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,lc*3*sizeof(uint16_t),iw.ptr(),GL_STATIC_DRAW);
		} else {


			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,co->index_id);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0,lc*3*sizeof(uint16_t),iw.ptr());
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0); //unbind

		co->len=lc;

	}

}

VS::InstanceType RasterizerStorageGLES3::get_base_type(RID p_rid) const {

	if (mesh_owner.owns(p_rid)) {
		return VS::INSTANCE_MESH;
	}

	if (multimesh_owner.owns(p_rid)) {
		return VS::INSTANCE_MULTIMESH;
	}

	if (immediate_owner.owns(p_rid)) {
		return VS::INSTANCE_IMMEDIATE;
	}

	if (light_owner.owns(p_rid)) {
		return VS::INSTANCE_LIGHT;
	}

	if (reflection_probe_owner.owns(p_rid)) {
		return VS::INSTANCE_REFLECTION_PROBE;
	}

	if (gi_probe_owner.owns(p_rid)) {
		return VS::INSTANCE_GI_PROBE;
	}

	return VS::INSTANCE_NONE;
}

bool RasterizerStorageGLES3::free(RID p_rid){

	if (render_target_owner.owns(p_rid)) {

		RenderTarget *rt = render_target_owner.getornull(p_rid);
		_render_target_clear(rt);
		Texture *t=texture_owner.get(rt->texture);
		texture_owner.free(rt->texture);
		memdelete(t);
		render_target_owner.free(p_rid);
		memdelete(rt);

	} else if (texture_owner.owns(p_rid)) {
		// delete the texture
		Texture *texture = texture_owner.get(p_rid);
		ERR_FAIL_COND_V(texture->render_target,true); //cant free the render target texture, dude
		info.texture_mem-=texture->total_data_size;
		texture_owner.free(p_rid);
		memdelete(texture);
	} else if (skybox_owner.owns(p_rid)) {
		// delete the skybox
		SkyBox *skybox = skybox_owner.get(p_rid);
		skybox_set_texture(p_rid,RID(),256);
		skybox_owner.free(p_rid);
		memdelete(skybox);

	} else if (shader_owner.owns(p_rid)) {

		// delete the texture
		Shader *shader = shader_owner.get(p_rid);


		if (shader->shader)
			shader->shader->free_custom_shader(shader->custom_code_id);

		if (shader->dirty_list.in_list())
			_shader_dirty_list.remove(&shader->dirty_list);

		while (shader->materials.first()) {

			Material *mat = shader->materials.first()->self();

			mat->shader=NULL;
			_material_make_dirty(mat);

			shader->materials.remove( shader->materials.first() );
		}

		//material_shader.free_custom_shader(shader->custom_code_id);
		shader_owner.free(p_rid);
		memdelete(shader);

	} else if (material_owner.owns(p_rid)) {

		// delete the texture
		Material *material = material_owner.get(p_rid);

		if (material->shader) {
			material->shader->materials.remove( & material->list );
		}

		if (material->ubo_id) {
			glDeleteBuffers(1,&material->ubo_id);
		}

		//remove from owners
		for (Map<Geometry*,int>::Element *E=material->geometry_owners.front();E;E=E->next()) {

			Geometry *g = E->key();
			g->material=RID();
		}
		for (Map<RasterizerScene::InstanceBase*,int>::Element *E=material->instance_owners.front();E;E=E->next()) {
			RasterizerScene::InstanceBase*ins=E->key();
			if (ins->material_override==p_rid) {
				ins->material_override=RID();
			}

			for(int i=0;i<ins->materials.size();i++) {
				if (ins->materials[i]==p_rid) {
					ins->materials[i]=RID();
				}
			}

		}

		material_owner.free(p_rid);
		memdelete(material);

	} else if (skeleton_owner.owns(p_rid)) {

		// delete the texture
		Skeleton *skeleton = skeleton_owner.get(p_rid);
		if (skeleton->update_list.in_list()) {
			skeleton_update_list.remove(&skeleton->update_list);
		}

		for (Set<RasterizerScene::InstanceBase*>::Element *E=skeleton->instances.front();E;E=E->next()) {
			E->get()->skeleton=RID();
		}

		skeleton_allocate(p_rid,0,false);
		skeleton_owner.free(p_rid);
		memdelete(skeleton);

	} else if (mesh_owner.owns(p_rid)) {

		// delete the texture
		Mesh *mesh = mesh_owner.get(p_rid);
		mesh->instance_remove_deps();
		mesh_clear(p_rid);

		mesh_owner.free(p_rid);
		memdelete(mesh);

	} else if (multimesh_owner.owns(p_rid)) {

		// delete the texture
		MultiMesh *multimesh = multimesh_owner.get(p_rid);
		multimesh->instance_remove_deps();

		multimesh_allocate(p_rid,0,VS::MULTIMESH_TRANSFORM_2D,VS::MULTIMESH_COLOR_NONE); //frees multimesh
		update_dirty_multimeshes();

		multimesh_owner.free(p_rid);
		memdelete(multimesh);
	} else if (immediate_owner.owns(p_rid)) {

		Immediate *immediate = immediate_owner.get(p_rid);
		immediate->instance_remove_deps();

		immediate_owner.free(p_rid);
		memdelete(immediate);
	} else if (light_owner.owns(p_rid)) {

		// delete the texture
		Light *light = light_owner.get(p_rid);
		light->instance_remove_deps();

		light_owner.free(p_rid);
		memdelete(light);

	} else if (reflection_probe_owner.owns(p_rid)) {

		// delete the texture
		ReflectionProbe *reflection_probe = reflection_probe_owner.get(p_rid);
		reflection_probe->instance_remove_deps();

		reflection_probe_owner.free(p_rid);
		memdelete(reflection_probe);

	} else if (gi_probe_owner.owns(p_rid)) {

		// delete the texture
		GIProbe *gi_probe = gi_probe_owner.get(p_rid);


		gi_probe_owner.free(p_rid);
		memdelete(gi_probe);
	} else if (gi_probe_data_owner.owns(p_rid)) {

		// delete the texture
		GIProbeData *gi_probe_data = gi_probe_data_owner.get(p_rid);

		print_line("dyndata delete");
		glDeleteTextures(1,&gi_probe_data->tex_id);
		gi_probe_owner.free(p_rid);
		memdelete(gi_probe_data);

	} else if (canvas_occluder_owner.owns(p_rid)) {


		CanvasOccluder *co = canvas_occluder_owner.get(p_rid);
		if (co->index_id)
			glDeleteBuffers(1,&co->index_id);
		if (co->vertex_id)
			glDeleteBuffers(1,&co->vertex_id);

		canvas_occluder_owner.free(p_rid);
		memdelete(co);

	} else if (canvas_light_shadow_owner.owns(p_rid)) {

		CanvasLightShadow *cls = canvas_light_shadow_owner.get(p_rid);
		glDeleteFramebuffers(1,&cls->fbo);
		glDeleteRenderbuffers(1,&cls->depth);
		glDeleteTextures(1,&cls->distance);
		canvas_light_shadow_owner.free(p_rid);
		memdelete(cls);
	} else {
		return false;
	}

	return true;
}

////////////////////////////////////////////


void RasterizerStorageGLES3::initialize() {

	config.render_arch=RENDER_ARCH_DESKTOP;
	//config.fbo_deferred=int(Globals::get_singleton()->get("rendering/gles3/lighting_technique"));

	config.system_fbo=0;


	//// extensions config
	///

	{

		int max_extensions=0;
		print_line("getting extensions");
		glGetIntegerv(GL_NUM_EXTENSIONS,&max_extensions);
		print_line("total "+itos(max_extensions));
		for(int i=0;i<max_extensions;i++) {
			const GLubyte *s = glGetStringi( GL_EXTENSIONS,i );
			if (!s)
				break;
			config.extensions.insert((const char*)s);
		}
	}

	config.shrink_textures_x2=false;
	config.use_fast_texture_filter=int(GlobalConfig::get_singleton()->get("rendering/quality/use_nearest_mipmap_filter"));
	config.use_anisotropic_filter = config.extensions.has("GL_EXT_texture_filter_anisotropic");

	config.s3tc_supported=config.extensions.has("GL_EXT_texture_compression_dxt1") || config.extensions.has("GL_EXT_texture_compression_s3tc") || config.extensions.has("WEBGL_compressed_texture_s3tc");
	config.etc_supported=config.extensions.has("GL_OES_compressed_ETC1_RGB8_texture");
	config.latc_supported=config.extensions.has("GL_EXT_texture_compression_latc");
	config.bptc_supported=config.extensions.has("GL_ARB_texture_compression_bptc");
#ifdef GLES_OVER_GL
	config.etc2_supported=false;
#else
	config.etc2_supported=true;
#endif
	config.pvrtc_supported=config.extensions.has("GL_IMG_texture_compression_pvrtc");
	config.srgb_decode_supported=config.extensions.has("GL_EXT_texture_sRGB_decode");



	config.anisotropic_level=1.0;
	config.use_anisotropic_filter=config.extensions.has("GL_EXT_texture_filter_anisotropic");
	if (config.use_anisotropic_filter) {
		glGetFloatv(_GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&config.anisotropic_level);
		config.anisotropic_level=MIN(int(GlobalConfig::get_singleton()->get("rendering/quality/anisotropic_filter_level")),config.anisotropic_level);
	}


	frame.clear_request=false;

	shaders.copy.init();

	{
		//default textures


		glGenTextures(1, &resources.white_tex);
		unsigned char whitetexdata[8*8*3];
		for(int i=0;i<8*8*3;i++) {
			whitetexdata[i]=255;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,resources.white_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_UNSIGNED_BYTE,whitetexdata);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,0);

		glGenTextures(1, &resources.black_tex);
		unsigned char blacktexdata[8*8*3];
		for(int i=0;i<8*8*3;i++) {
			blacktexdata[i]=0;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,resources.black_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_UNSIGNED_BYTE,blacktexdata);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,0);

		glGenTextures(1, &resources.normal_tex);
		unsigned char normaltexdata[8*8*3];
		for(int i=0;i<8*8*3;i+=3) {
			normaltexdata[i+0]=128;
			normaltexdata[i+1]=128;
			normaltexdata[i+2]=255;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,resources.normal_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_UNSIGNED_BYTE,normaltexdata);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,0);


		glGenTextures(1, &resources.aniso_tex);
		unsigned char anisotexdata[8*8*3];
		for(int i=0;i<8*8*3;i+=3) {
			anisotexdata[i+0]=255;
			anisotexdata[i+1]=128;
			anisotexdata[i+2]=0;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,resources.aniso_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_UNSIGNED_BYTE,anisotexdata);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,0);

	}

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&config.max_texture_image_units);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&config.max_texture_size);

#ifdef GLES_OVER_GL
	config.use_rgba_2d_shadows=false;
#else
	config.use_rgba_2d_shadows=true;
#endif


	//generic quadie for copying

	{
		//quad buffers

		glGenBuffers(1,&resources.quadie);
		glBindBuffer(GL_ARRAY_BUFFER,resources.quadie);
		{
			const float qv[16]={
				-1,-1,
				 0, 0,
				-1, 1,
				 0, 1,
				 1, 1,
				 1, 1,
				 1,-1,
				 1, 0,
			};

			glBufferData(GL_ARRAY_BUFFER,sizeof(float)*16,qv,GL_STATIC_DRAW);
		}

		glBindBuffer(GL_ARRAY_BUFFER,0); //unbind


		glGenVertexArrays(1,&resources.quadie_array);
		glBindVertexArray(resources.quadie_array);
		glBindBuffer(GL_ARRAY_BUFFER,resources.quadie);
		glVertexAttribPointer(VS::ARRAY_VERTEX,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(VS::ARRAY_TEX_UV,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,((uint8_t*)NULL)+8);
		glEnableVertexAttribArray(4);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0); //unbind
	}

	//generic quadie for copying without touching skybox

	{
		//transform feedback buffers
		uint32_t xf_feedback_size = GLOBAL_DEF("rendering/buffers/blend_shape_max_buffer_size_kb",4096);
		for(int i=0;i<2;i++) {

			glGenBuffers(1,&resources.transform_feedback_buffers[i]);
			glBindBuffer(GL_ARRAY_BUFFER,resources.transform_feedback_buffers[i]);
			glBufferData(GL_ARRAY_BUFFER,xf_feedback_size*1024,NULL,GL_STREAM_DRAW);
		}

		shaders.blend_shapes.init();

		glGenVertexArrays(1,&resources.transform_feedback_array);

	}

	shaders.cubemap_filter.init();
	shaders.particles.init();

	glEnable(_EXT_TEXTURE_CUBE_MAP_SEAMLESS);

	frame.count=0;
	frame.prev_tick=0;
	frame.delta=0;
	config.keep_original_textures=false;
}

void RasterizerStorageGLES3::finalize() {

	glDeleteTextures(1, &resources.white_tex);
	glDeleteTextures(1, &resources.black_tex);
	glDeleteTextures(1, &resources.normal_tex);

}


RasterizerStorageGLES3::RasterizerStorageGLES3()
{

}
