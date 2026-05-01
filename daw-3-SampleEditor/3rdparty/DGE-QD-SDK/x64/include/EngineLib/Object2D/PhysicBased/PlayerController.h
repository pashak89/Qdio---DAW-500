#include "../../Physics/2D/Physics2D.h"
namespace DGE
{
	using namespace DGE::Math;
	using namespace DGE::IO;
	using namespace DGE::GFX;

	namespace Objects2D
	{
		class _2DObjectsManager;
		namespace PhysicBase
		{
#if defined(DGE_UseGraphicAPI)
			class DGE_API PlayerController : public DynamicObject
			{
			protected:
				uint index = 0;
				float timeElapsed = 0;
				uint numberPictureinRow;
				bool m_hasContact;

			public:
				PlayerController(Renderer* pRenderer, Bound BoundImage, PhysicMaterial physicMaterial, uint numberPictureinRow = 8);
				PlayerController(Renderer* pRenderer, Bound BoundImage, PhysicMaterial physicMaterial, Bound BoundPhysicsShape, uint numberPictureinRow = 8);
				virtual ~PlayerController();
				virtual void Render();
				virtual void Update(float elapsed);
				virtual void Move(Vector2D direction);
				virtual void AddForce(Vector2D direction);
				virtual void EnableGravity(bool flag);
				virtual bool HasContact();

				virtual void BeginContact(Physics2dData* object) override;

				virtual void EndContact(Physics2dData* object) override;

			};
#endif
		}
	}
}