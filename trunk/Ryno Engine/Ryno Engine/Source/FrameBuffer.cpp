#include "FrameBuffer.h"
#include "Log.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>

namespace Ryno {

	FrameBuffer::FrameBuffer(U32 width, U32 height){
		init(width, height);
	}


	void FrameBuffer::init(U32 width, U32 height){

		// Create and bind the FBO
		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

		// Create the frame buffer textures
		glGenTextures(FRAME_NUM_TEXTURES, m_textures);
		glGenTextures(1, &m_depth_texture);
		glGenTextures(1, &m_final_texture);

		////bind position texture
		//glBindTexture(GL_TEXTURE_2D, m_textures[FRAME_TEXTURE_TYPE_POSITION]);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textures[FRAME_TEXTURE_TYPE_POSITION], 0);

		//bind color texture
		glBindTexture(GL_TEXTURE_2D, m_textures[FRAME_TEXTURE_TYPE_DIFFUSE]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textures[FRAME_TEXTURE_TYPE_DIFFUSE], 0);

		//bind normal texture
		glBindTexture(GL_TEXTURE_2D, m_textures[FRAME_TEXTURE_TYPE_NORMAL]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_FLOAT, nullptr);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_textures[FRAME_TEXTURE_TYPE_NORMAL], 0);

		//bind depth texture
		glBindTexture(GL_TEXTURE_2D, m_textures[FRAME_TEXTURE_TYPE_DEPTH]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, nullptr);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_textures[FRAME_TEXTURE_TYPE_DEPTH], 0);

		

		//Bind depth texture (with 8 bits for stencil)
		glBindTexture(GL_TEXTURE_2D, m_depth_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, nullptr);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);

		//Bind final texture
		glBindTexture(GL_TEXTURE_2D, m_final_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_final_texture, 0);

		//Check if ok
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			Log::FatalError("FB error.");
	
		}

		// Restore default FBO
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


	}

	void FrameBuffer::start_frame()
	{
		//Binds the custom framebuffer, and then clear the previous final_texture
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT4);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void FrameBuffer::bind_for_geometry_pass()
	{
		//Binds custom buffer, specify draw buffers, and set them to draw
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2 };

		glDrawBuffers(FRAME_NUM_TEXTURES, DrawBuffers);
	}

	void FrameBuffer::bind_for_stencil_pass()
	{
		//Disable all draw buffers, cause it just wants to get depth and stencil.
		//Without this, the drawing would override geometry pass (because the fbo is the same)
		glDrawBuffer(GL_NONE);
	}


	void FrameBuffer::bind_for_light_pass()
	{
		//Draw in the final_texture of fbo, not yet in the screen buffer
		glDrawBuffer(GL_COLOR_ATTACHMENT4);

		//Bind the textures correspondent to the buffers of the gbuffer.
		//The light shader needs textures position (like GL_TEXTURE0), not texture ids.
		//So we need to bind the id to the location.

		for (U8 i = 0; i < FRAME_NUM_TEXTURES; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		}
	}

	void FrameBuffer::bind_for_final_pass()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT4);
	}

	 
}
	
