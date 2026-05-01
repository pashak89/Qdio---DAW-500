/* Copyright (C) 2025, Abbas Aliakbari - All Rights Reserved
 * SPDX-License-Identifier: MIT
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: a.aliakbari91@gmail.com
 *
 * Written by Abbas Aliakbari
 *
 * @section DESCRIPTION
 *
 * API Configurator header file is using for detecting available features and libraries
 */
#pragma once

#if DGE_Sound_API==DGE_Use_DirectMusic && defined(DGE_UseSoundAPI)
using namespace DGE::Resource;
using namespace DGE::Core::Math;
namespace DGE
{
	namespace SFX
	{
		namespace DirectMusic
		{
			//-----------------------------------------------------------------------------
			// Sound System Class
			//-----------------------------------------------------------------------------
			class DGE_API SoundSystemDM :public SoundSystem
			{
			public:
				SoundSystemDM(float scale = 1.0f);
				virtual ~SoundSystemDM();

				void UpdateListener(Vector3D forward, Vector3D position, Vector3D velocity);

				void GarbageCollection();

				void SetVolume(long volume);

				void* GetLoader();
				void* GetPerformance();

			private:
				float m_scale; // Unit scale in meters/unit.
				IDirectMusicLoader8 *m_loader; // DirectMusic loader.
				IDirectMusicPerformance8 *m_performance; // DirectMusic performance.
				IDirectSound3DListener8 *m_listener; // DirectSound 3D listener.
			};


			//-----------------------------------------------------------------------------
			// Sound Class
			//-----------------------------------------------------------------------------
			class DGE_API SoundDM :public Sound
			{
			public:
				SoundDM(DChar* filename);
				virtual ~SoundDM();

				void Play(bool loop = false, unsigned long flags = DMUS_SEGF_AUTOTRANSITION);

				IDirectMusicSegment8 *GetSegment();

			private:
				IDirectMusicSegment8* m_segment; // DirectMusic segment for the sound.
				IDirectMusicLoader8* m_Loader;
				IDirectMusicPerformance8* m_Performance;
				bool m_Loaded=false;
			};


			//-----------------------------------------------------------------------------
			// Audio Path 3D Class
			//-----------------------------------------------------------------------------
			class DGE_API AudioPath3D_DM :public AudioPath3D
			{
			public:
				AudioPath3D_DM();
				virtual ~AudioPath3D_DM();

				void SetPosition(Vector3D position);
				void SetVelocity(Vector3D velocity);
				void SetMode(unsigned long mode);

				virtual void Play(IDirectMusicSegment8 *segment, bool loop = false, BaseType::uint32 flags = DMUS_SEGF_SECONDARY);
				virtual void Play(void *segment, bool loop = false, BaseType::uint32 flags = DMUS_SEGF_SECONDARY);

			private:
				IDirectMusicAudioPath8 *m_audioPath; // DirectMusic audio path for 3D playback.
				IDirectSound3DBuffer8 *m_soundBuffer; // Pointer to the audio path's sound buffer.
				IDirectMusicPerformance8* m_Performance;
			};
		}
	}
}
#endif