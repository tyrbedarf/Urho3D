#pragma once

#include "Math/Vector3d.h"
#include "Voxel.h"
#include "Core/Object.h"

namespace Urho3D
{
    class IChunk : public Object
    {
		URHO3D_OBJECT(IChunk, Object)

	public:
		IChunk(Context* ctx) : Object(ctx)
		{

		}

        /// <summary>
        /// Position of the chunk in double precision coordinates.
        /// This is not the position of the chunk in the scene, it will
        /// be converted and updated during rendering.
        /// Voxer uses a floating origin approach for rendering.
        /// </summary>
        virtual const Vector3d& GetWorldPosition() const = 0;

        /// All of the boolean fields here exist to handle multithreading.
        /// A chunk shall not be initialized twice but we must be able to distinguish
        /// whether initialization is done or not.
        /// The same goes for surface extraction.
        /// We do not want to remove chunks from our internal list while it still builds.
        /// So as long as the value IsMeshInGame is not true, we do not remove a chunk
        /// from our internal representation to avoid having meshes in game that are not
        /// being removed.

        /// <summary>
        /// True if a chunk has been initialized
        /// </summary>
		virtual bool GetInitialized() const = 0;

        /// <summary>
        /// True, when a chunk is beeing initialized or is initialized.
        /// </summary>
		virtual bool Initializing() const = 0;

        /// <summary>
        /// True when surface extraction is done.
        /// </summary>
		virtual bool Meshed() const = 0;

        /// <summary>
        /// True, when surface extraction has started or is done.
        /// </summary>
		virtual bool Meshing() const = 0;

        /// <summary>
        /// True after the chunk has been spawned.
        /// </summary>
		virtual bool IsMeshInGame() const = 0;
		virtual void SetMeshInGame(bool value) = 0;


        /// <summary>
        /// Chunks will be initialized and their surface will be extracted in a certain order.
        /// The marker is essentially the squared distance between the chunk and the player
        /// position that triggered its creation. The smaller the value the sooner the chunk
        /// will be created.
        /// </summary>
		virtual double GetInitializationMarker() const = 0;
		virtual void SetInitializationMarker(double value) = 0;

        /// <summary>
        /// True, if a chunk has no neighbor at a certain side after initialization phase.
        /// This is needed to identify chunks that need to be recreated when new chunks spawn in.
        /// </summary>
		virtual bool IsBorderChunk() const = 0;

        /// <summary>
        /// Chunks are not beeing destroyed to save the overhead of allocating and deallocating memory (Object Pooling).
        /// They are always beeing reused. This method has to be called to resuse a chunk and reset all of its
        /// internal values.
        /// </summary>
        /// <param name="pos"></param>
		virtual void Reset(Vector3d pos) = 0;

        /// <summary>
        /// Called while spawning chunks to notify the chunk of existing neighbors.
        /// </summary>
        /// <param name="x">A value between -1 and 1 telling the chunk wether the neighbor is to the left or the right</param>
        /// <param name="y">A value between -1 and 1 telling the chunk wether the neighbor is above or below</param>
        /// <param name="z">A value between -1 and 1 telling the chunk wether the neighbor is in front or behind</param>
        /// <param name="c">The neighbor</param>
		virtual void SetNeighbor(int x, int y, int z, IChunk* c) = 0;

        /// <summary>
        /// Initialize a chunk and potentially all of its neighbors.
        /// </summary>
		virtual void Initialize() = 0;

        /// <summary>
        /// Create the implizit surface represented by the given chunk of data.
        /// In other words, create the visual and collider mesh.
        /// </summary>
		virtual void CreateMesh() = 0;

        /// <summary>
        /// Called when the chunk should be removed. Should handle serialization and
        /// then call VoxerSystemProxy to remove the mesh from the game.
        /// </summary>
		virtual void Despawn() = 0;

        /// <summary>
        /// Set a single voxel inside a chunk. If a position points to
        /// a position outside the chunk a will pass the request on to the
        /// right neighbor, if it has been already initialized.
        /// Avoid using this method during initialization since it is computationally "heavy".
        /// </summary>
        /// <param name="data"></param>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="z"></param>
		virtual void Set(const Voxel& data, int x, int y, int z) = 0;

        /// <summary>
        /// Retrieve a certain voxel from a chunk. If the position of the voxel
        /// is outside the current chunk the request will be passed to neighboring
        /// chunks, if they have been initialized.
        /// Avoid using this method during initialization since it is computationally "heavy".
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="z"></param>
        /// <returns>The voxel in question and true, or default voxel and false</returns>
       virtual Voxel& Get(int x, int y, int z, bool& found) = 0;

        /// <summary>
        /// Return true, if the surface can be extracted from the current chunk.
        /// This means the chunk and all of its neighbors have been initialized.
        /// </summary>
        /// <returns></returns>
		virtual bool CanExtractSurface() const = 0;
	};
}