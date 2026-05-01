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

// TODO : error in output log when exit from game: Foundation destruction failed due to pending module references. Close/release all depending modules first.
namespace DGE
{
	namespace Physic
	{
		using namespace std;

		class DGE_API Px_shape
		{
#if DGE_Physic_API==DGE_Use_PhysXEngine
			PxActor *orgactor = nullptr;
			friend class Physics;
			Px_shape(float sangle, PxActor *ac);
		public:
			Px_shape(){}
			~Px_shape();
			void		Release();
			void		AddForce(Vector3D amount);
			void		SetMass(float amount);
			void		SetTrigger();
			Vector3D	GetPosition();
			void		GetPosition(Vector3D& vec);
			Vector4D	GetRotation();
			void		GetRotation(Vector4D& vec);
			void		GetTransform(Matrix& matrix);
			Matrix		GetTransform();
#endif
		};

		enum PhysicMaterials
		{
			Wood=0, Iron, Rubber, MaxBounciness,Count
		};

		class DGE_API Physics
		{
#if DGE_Physic_API==DGE_Use_PhysXEngine

			Vector3D				m_gravity;
			PxPhysics*				m_physicsSDK=nullptr;			//Instance of PhysX SDK
			PxFoundation*			m_foundation = nullptr;			//Instance of singleton foundation SDK class
			PxScene*				m_scene = nullptr;				//Instance of PhysX Scene				
			PxCudaContextManager*	m_contextManager = nullptr;
			PxProfileZoneManager*	m_profileZoneManager = nullptr;
			PxDefaultErrorCallback	m_defaultErrorCallback;		//Instance of default implementation of the error callback
			PxDefaultAllocator		m_defaultAllocatorCallback;	//Instance of default implementation of the allocator interface required by the SDK

			const char* pvd_host_ip = "127.0.0.1"; // IP of local PC machine PVD
			int port = 5425; // TCP port to connect to, where PVD is listening
			debugger::comm::PvdConnection* m_pvdConnection = nullptr;

			double m_elapseTime=0;
			double m_timeStepUpdate = 1.0f / 60.0f;
			PxMaterial *m_arrayMaterials[PhysicMaterials::Count];
			void InitializePhysicMaterial();



		public:
			Physics(Vector3D gravity = Vector3D(0, -9.98f, 0), float TimeStep = 1.0f / 60.0f);
			~Physics();

			//sPosition: 
			Px_shape createplane(Vector3D sPosition, Vector3D sRotaton, float sangle, PhysicMaterials mat, bool isstatic = true);
			Px_shape createbox(Vector3D sPosition, Vector3D sRotaton, float sangle, PhysicMaterials mat, float hx, float hy, float hz, bool isstatic = true);
			Px_shape createsphere(Vector3D sPosition, Vector3D sRotaton, float sangle, PhysicMaterials mat, float r, bool isstatic = true);
			Px_shape createcapsule(Vector3D sPosition, Vector3D sRotaton, float sangle, PhysicMaterials mat, float hh, float r, bool isstatic = true);


			void StepPhysX(float elapse);

			void ConnetToPVD();
#endif
		};

	}
}
