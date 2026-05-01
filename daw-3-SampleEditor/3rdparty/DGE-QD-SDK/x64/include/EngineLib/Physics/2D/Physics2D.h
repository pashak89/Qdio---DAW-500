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

namespace DGE
{
	namespace Physic
	{
		namespace _2D
		{
			using namespace std;
			using namespace DGE::Core::Math;
#if DGE_2DPhysic_API==DGE_Use_Box2DEngine
			using Physics2dBody = b2Body;
			//Translate units between Render World and Physics World
			extern DGE_Extern float fPRV; ///< Physics world rescale value
			inline float PW2RW(float x){ return x*fPRV; }; ///< Physics World to Render World units.
			inline float RW2PW(float x){ return x / fPRV; }; ///< Render World to Physics World units.
			inline float RW2PW(int x){ return (float) x / fPRV; }; ///< Render World to Physics World units.

			enum class DGE_API Type2dBody
			{
				StaticBody = 0,
				KinematicBody,
				DynamicBody
			};
			struct Physics2dData
			{
				void* userData;
				dstring id;
			};

			struct DGE_API Physic2dObject
			{
			protected:
				friend class Physics2D;
				friend class Physics2dContactListener;
				Physics2dBody* m_physicBody;
				virtual void BeginContact(Physics2dData* object) = 0;

				virtual void EndContact(Physics2dData* object)=0;
			public:
			};

			class DGE_API Physics2dContactListener:public b2ContactListener
			{
				virtual void BeginContact(b2Contact* contact) override;

				virtual void EndContact(b2Contact* contact) override;

				virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

				virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

			};

			class DGE_API Physics2D
			{
				Physics2dContactListener m_contactListener;
				b2World* m_world;
				uint m_objectCounter;
				
			public:
				bool m_exited;
				Physics2D(Vector2D gravity = Vector2D(0, 10));
				virtual~Physics2D();
				Physics2dBody* CreatePhysicObject(Vector2D position, ShapeProperties &shapProperties, Type2dBody type);

				void Update(float timeStep=1.0f/60.0f, int velocityIterations = 6, int positionIteration = 2);

				void SetGravity(Vector2D gravity);
			};
#endif
		}
	}
}
