﻿using BulletSharp;
using Mogre;
using Ponykart.Physics;
using PonykartParsers;

namespace Ponykart.Actors {
	public class ShapeComponent : IThingComponent {
		public int ID { get; protected set; }
		public string Name { get; protected set; }
		public CollisionShape Shape { get; protected set; }
		public Matrix4 Transform { get; protected set; }

		public ShapeComponent(LThing lthing, ThingBlock template, ShapeBlock block) {
			ID = IDs.New;
			var sceneMgr = LKernel.GetG<SceneManager>();

			Name = block.GetStringProperty("name", template.ThingName);
			Shape = block.Shape;
			Transform = block.Transform;

			if (block.EnumTokens["type"] == ThingEnum.Hull) {
				// TODO: this stuff
				/*string name = block.GetStringProperty("name", "");

				if (name != "" && File.Exists(Settings.Default.BulletFileLocation + name + Settings.Default.BulletFileExtension)) {
					Shape = LKernel.GetG<PhysicsMain>().ImportCollisionShape(name);
				}
				else {*/
					string meshName = block.GetStringProperty("mesh", null);

					// TODO: need a better way of loading a mesh
					Entity ent = LKernel.GetG<SceneManager>().CreateEntity(meshName);

					Shape = OgreToBulletMesh.ConvertToConvexHull(ent.GetMesh(), Transform.GetTrans(), Transform.ExtractQuaternion(), Vector3.UNIT_SCALE);

					LKernel.GetG<SceneManager>().DestroyEntity(ent);
					ent.Dispose();

					/*LKernel.GetG<PhysicsMain>().SerializeShape(Shape, name);
				}*/
			}
			

		}

		public void Dispose() { }
	}
}