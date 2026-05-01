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
#pragma region Physic API Configourator
#ifdef DGE_UsePhysicAPI
		#if DGE_Physic_API==DGE_Use_PhysXEngine
			#pragma region PhysXEngine Library

				#include <PhysX\PxPhysicsAPI.h>
				#ifdef _DEBUG //If in 'Debug' load libraries for debug mode 
					#ifdef Dynamic
						#pragma comment(lib, "PhysX3DEBUG_x86.lib")				//Always be needed  
						#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")		//Always be needed
						#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")		//PhysX extended library 
						#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib") //For PVD only 
						#pragma comment(lib, "PhysXProfileSDKDEBUG.lib")	
						#pragma comment(lib, "PxTaskDEBUG.lib")	
					#else
						#pragma comment(lib, "PhysX3DEBUG_x86.lib")				//Always be needed  
						#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")		//Always be needed
						#pragma comment(lib, "PhysX3ExtensionsDEBUGs.lib")		//PhysX extended library 
						#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib") //For PVD only 
						#pragma comment(lib, "PhysXProfileSDKDEBUG.lib")	
						#pragma comment(lib, "PxTaskDEBUG.lib")	
					#endif
				#else //Else load libraries for 'Release' mode
					#pragma comment(lib, "PhysX3PROFILE_x86.lib")	
					#pragma comment(lib, "PhysX3CommonPROFILE_x86.lib") 
					#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
					#pragma comment(lib, "PhysXVisualDebuggerSDKPROFILE.lib")
					#pragma comment(lib, "PhysXProfileSDKPROFILE.lib")	
					#pragma comment(lib, "PxTaskPROFILE.lib")	
				#endif
				#pragma warning(disable : 4099)
				using namespace physx;
			#pragma endregion
		#else

		#endif

#if defined(DGE_2DPhysic_API) && DGE_2DPhysic_API == DGE_Use_Box2DEngine

#include <Box2D/Box2D.h>
#undef uint32
#ifdef Dynamic
#ifdef _DEBUG						// If in 'Debug' load libraries for debug mode
#pragma comment(lib, "Box2DDD.lib") // Always be needed
#else								// Else load libraries for 'Release' mode
#pragma comment(lib, "Box2DDR.lib") // Always be needed
#endif

#else
#ifdef _DEBUG						// If in 'Debug' load libraries for debug mode
#pragma comment(lib, "Box2DSD.lib") // Always be needed
#else								// Else load libraries for 'Release' mode
#pragma comment(lib, "Box2DSR.lib") // Always be needed
#endif
#endif
#else
#endif
#endif
#pragma endregion
