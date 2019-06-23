#pragma once

#include "../../Math/Vector3i.h"
#include "../../Math/Vector3.h"
#include "FlatArray2D.h"
#include "../../Core/Object.h"
#include "../../Helper/Vector3Helper.h"
#include <sstream>

/*
		   6-------------------	7
		  /					   /|
		 / |   			      /	|
		/  |   			     /	|
	   /   |  			    /	|
	  /    | 			   /	|
	 /	   |			  /		|
	2--------------------5		|
	|	   |			 |		|
	|      3------------ | -----4
	|     /				 |	   /
	|    /				 |	  /
	|   / 				 |	 /
	|  /				 |  /
	| /					 | /
	|/					 |/
	0--------------------1

	Index	Offset		Cube Index      Vertex Key
	0		( 0, 0, 0)	  1             ( 0,  0,  0)
	1		( 1, 0, 0)	  2             (-1,  0,  0)
	2		( 0, 1, 0)	  4             ( 0, -1,  0)
	3		( 0, 0, 1)	  8             ( 0,  0, -1)

	4		( 1, 0, 1)	 16             (-1,  0, -1)
	5		( 1, 1, 0)	 32             (-1, -1,  0)
	6		( 0, 1, 1)	 64             ( 0, -1, -1)
	7		( 1, 1, 1)	128             (-1, -1, -1)
*/

namespace Urho3D
{
	class SurfaceData : public Object
	{
		URHO3D_OBJECT(SurfaceData, Object)

	private:
		void InitCube()
		{
			Cube[0] = Vector3i(0, 0, 0);
			Cube[1] = Vector3i(1, 0, 0);
			Cube[2] = Vector3i(0, 1, 0);
			Cube[3] = Vector3i(0, 0, 1);

			Cube[4] = Vector3i(1, 0, 1);
			Cube[5] = Vector3i(1, 1, 0);
			Cube[6] = Vector3i(0, 1, 1);
			Cube[7] = Vector3i(1, 1, 1);

			for (int i = 0; i < 8; i++)
			{
				Vertices[i] = Cube[i] * -1;
			}
		}

		void InitFaces()
		{
			Faces.Set(TOP, 0, 4);   Faces.Set(BOTTOM, 0, 1);
			Faces.Set(TOP, 1, 0);   Faces.Set(BOTTOM, 1, 0);
			Faces.Set(TOP, 2, 1);   Faces.Set(BOTTOM, 2, 4);
			Faces.Set(TOP, 3, 3);   Faces.Set(BOTTOM, 3, 4);
			Faces.Set(TOP, 4, 0);   Faces.Set(BOTTOM, 4, 0);
			Faces.Set(TOP, 5, 4);   Faces.Set(BOTTOM, 5, 3);
			Faces.Set(TOP, 6, 0);   Faces.Set(BOTTOM, 6, 2);

			Faces.Set(BACK, 0, 5);   Faces.Set(FRONT, 0, 1);
			Faces.Set(BACK, 1, 0);   Faces.Set(FRONT, 1, 0);
			Faces.Set(BACK, 2, 1);   Faces.Set(FRONT, 2, 5);
			Faces.Set(BACK, 3, 2);   Faces.Set(FRONT, 3, 5);
			Faces.Set(BACK, 4, 0);   Faces.Set(FRONT, 4, 0);
			Faces.Set(BACK, 5, 5);   Faces.Set(FRONT, 5, 2);
			Faces.Set(BACK, 6, 3);   Faces.Set(FRONT, 6, 0);

			Faces.Set(RIGHT, 0, 6);   Faces.Set(LEFT, 0, 0);
			Faces.Set(RIGHT, 1, 0);   Faces.Set(LEFT, 1, 6);
			Faces.Set(RIGHT, 2, 3);   Faces.Set(LEFT, 2, 3);
			Faces.Set(RIGHT, 3, 0);   Faces.Set(LEFT, 3, 6);
			Faces.Set(RIGHT, 4, 6);   Faces.Set(LEFT, 4, 0);
			Faces.Set(RIGHT, 5, 2);   Faces.Set(LEFT, 5, 2);
			Faces.Set(RIGHT, 6, 0);   Faces.Set(LEFT, 6, 1);
		}

		static Vector2 vec(float u, float v)
		{
			return Vector2(u, v);
		}

		void InitCubeToFaceLookup()
		{
			//  0
			CubeIndexToFace[0][0] = STOP;

			//  1
			CubeIndexToFace[1][0] = TOP;
			CubeIndexToFace[1][1] = FRONT;
			CubeIndexToFace[1][2] = RIGHT;
			CubeIndexToFace[1][3] = STOP;

			//  2
			CubeIndexToFace[2][0] = LEFT;
			CubeIndexToFace[2][1] = STOP;

			//  3
			CubeIndexToFace[3][0] = TOP;
			CubeIndexToFace[3][1] = FRONT;
			CubeIndexToFace[3][2] = STOP;

			//  4
			CubeIndexToFace[4][0] = BOTTOM;
			CubeIndexToFace[4][1] = STOP;

			//  5
			CubeIndexToFace[5][0] = FRONT;
			CubeIndexToFace[5][1] = RIGHT;
			CubeIndexToFace[5][2] = STOP;

			//  6
			CubeIndexToFace[6][0] = BOTTOM;
			CubeIndexToFace[6][1] = LEFT;
			CubeIndexToFace[6][2] = STOP;

			//  7
			CubeIndexToFace[7][0] = FRONT;
			CubeIndexToFace[7][1] = STOP;

			//  8
			CubeIndexToFace[8][0] = BACK;
			CubeIndexToFace[8][1] = STOP;

			//  9
			CubeIndexToFace[9][0] = TOP;
			CubeIndexToFace[9][1] = RIGHT;
			CubeIndexToFace[9][2] = STOP;

			// 10
			CubeIndexToFace[10][0] = BACK;
			CubeIndexToFace[10][1] = LEFT;
			CubeIndexToFace[10][2] = STOP;

			// 11
			CubeIndexToFace[11][0] = TOP;
			CubeIndexToFace[11][1] = STOP;

			// 12
			CubeIndexToFace[12][0] = BACK;
			CubeIndexToFace[12][1] = BOTTOM;
			CubeIndexToFace[12][2] = STOP;

			// 13
			CubeIndexToFace[13][0] = LEFT;
			CubeIndexToFace[13][1] = STOP;

			// 14
			CubeIndexToFace[14][0] = RIGHT;
			CubeIndexToFace[14][1] = BOTTOM;
			CubeIndexToFace[14][2] = BACK;
			CubeIndexToFace[14][3] = STOP;

			// 15
			CubeIndexToFace[15][0] = STOP;

			/*std::stringstream ss;
			ss << "\nCubeIndexToFace:\n";
			for (int i = 0; i < 16; i++)
			{
				ss
					<< CubeIndexToFace[i][0] << " "
					<< CubeIndexToFace[i][1] << " "
					<< CubeIndexToFace[i][2] << " "
					<< CubeIndexToFace[i][3] << "\n";
			}

			ss << "\n";*/

			// Prepare the actual lookup tables for our vertices.
			for (int i = 0; i < 256; i++)
			{
				int index = i & 15;
				if (index > 15)
				{
					continue;
				}

				int quad = 0;
				while (CubeIndexToFace[index][quad] != STOP)
				{
					// Vertices
					CubeIndexToFaceLookup.Set(index, (quad * 7) + 0, Faces.Get(CubeIndexToFace[index][quad], 0));
					CubeIndexToFaceLookup.Set(index, (quad * 7) + 1, Faces.Get(CubeIndexToFace[index][quad], 1));
					CubeIndexToFaceLookup.Set(index, (quad * 7) + 2, Faces.Get(CubeIndexToFace[index][quad], 2));

					CubeIndexToFaceLookup.Set(index, (quad * 7) + 3, Faces.Get(CubeIndexToFace[index][quad], 3));
					CubeIndexToFaceLookup.Set(index, (quad * 7) + 4, Faces.Get(CubeIndexToFace[index][quad], 4));
					CubeIndexToFaceLookup.Set(index, (quad * 7) + 5, Faces.Get(CubeIndexToFace[index][quad], 5));

					CubeIndexToFaceLookup.Set(index, (quad * 7) + 6, Faces.Get(CubeIndexToFace[index][quad], 6));

					quad++;
				}
			}

			/*for (int i = 0; i < 16; i++)
			{
				ss << "Index: " << i << "\n";
				int quad = 0;
				while (CubeIndexToFace[i][quad] != STOP)
				{
					ss
						<< CubeIndexToFaceLookup.Get(i, (quad * 7) + 0) << " "
						<< CubeIndexToFaceLookup.Get(i, (quad * 7) + 1) << " "
						<< CubeIndexToFaceLookup.Get(i, (quad * 7) + 2) << " "
						<< CubeIndexToFaceLookup.Get(i, (quad * 7) + 3) << " "
						<< CubeIndexToFaceLookup.Get(i, (quad * 7) + 4) << " "
						<< CubeIndexToFaceLookup.Get(i, (quad * 7) + 5) << "\n";

					quad++;
				}
			}

			URHO3D_LOGDEBUG(ss.str().c_str());*/
		}

		void InitEdges()
		{
			Edges.Set(0, 0, 2); Edges.Set(0, 1, 1); Edges.Set(0, 2, 3);
			Edges.Set(1, 0, 0); Edges.Set(1, 1, 5); Edges.Set(1, 2, 4);
			Edges.Set(2, 0, 0); Edges.Set(2, 1, 5); Edges.Set(2, 2, 6);
			Edges.Set(3, 0, 0); Edges.Set(3, 1, 4); Edges.Set(3, 2, 6);
			Edges.Set(4, 0, 1); Edges.Set(4, 1, 3); Edges.Set(1, 2, 7);
			Edges.Set(5, 0, 1); Edges.Set(5, 1, 2); Edges.Set(5, 2, 7);
			Edges.Set(6, 0, 2); Edges.Set(6, 1, 3); Edges.Set(6, 2, 7);
			Edges.Set(7, 0, 4); Edges.Set(7, 1, 5); Edges.Set(7, 2, 6);
		}

		void InitPointTable()
		{
			for (int i = 0; i < 8; i++)
			{
				PointTableVertices[i] = Vector3(
					Cube[i].x,
					Cube[i].y,
					Cube[i].z) * mVoxelSize;
			}

			for (int i = 0; i < 256; i++)
			{
				PointTable[i] = CalcCentroid(i);
			}
		}

		Vector3 CalcCentroid(int cubeindex)
		{
			int cuts = 0;
			Vector3 pos(0,0,0);
			for (int i = 0; i < 8; i++)
			{
				if ((cubeindex & (1 << i)) != 0)
				{
					/* Check edges */
					for (int j = 0; j < 3; j++)
					{
						if ((cubeindex & (1 << Edges.Get(i, j))) == 0)
						{
							cuts++;
							auto tmp0 = PointTableVertices[i];
							auto tmp2 = PointTableVertices[Edges.Get(i, j)];
							pos += (tmp0 + tmp2) * 0.5f;
						}
					}
				}
			}

			return (pos * (1.0f / (float)cuts));
		}
	public:
		// Sides of a cube
		const int TOP		= 0;
		const int BOTTOM	= 1;
		const int BACK		= 2;
		const int FRONT		= 3;
		const int RIGHT		= 4;
		const int LEFT		= 5;
		const int STOP		= -1;	// Marks the end of a list of triangles

		// Encodes the positions of a voxel cube inside a chunk relative
		// to the "center" (vertex 0).
		Vector3i Cube[8];

		// Encodes which vertices we need to build a surface.
		// We connect each vertex to vertices that occured before
		// the current position.
		Vector3i Vertices[8];

		// The cube vertices converted to Vector3 taking voxel size
		// into account.
		Vector3 PointTableVertices[8];

		// Contains two triangles per side of a vertex cube.
		// The last value is the voxel that provids the material for the quad.
		FlatArray2D<int> Faces;

		// Contains the texture positions for each vertex of a
		// quad.
		/*FlatArray2D<Vector2> Uvs;*/

		// Encodes which sides of a cube we need according to the
		// short cube index. The short cube index tells us which edges
		// are cut by the surface going from the cube center (vertex 0) to
		// its direct neighbors.
		int CubeIndexToFace[16][4];

		// Same as above, but rather than pointing to faces this is created
		// during startup and points to single vertices as tuples of two
		// triangles (six indices) and one material index.
		FlatArray2D<int> CubeIndexToFaceLookup;
		// public FlatArray2D<Vector2> CubeIndexToUvsLookup;

		// Encodes, how the vertices inside the voxel cube are connected to
		// one another.
		FlatArray2D<int> Edges;

		// Maps each cube index to a position inside the voxel cube.
		// Will be calculated during startup.
		Vector3 PointTable[256];

		// Size of a voxel
		float mVoxelSize;

		/// Layout of voxels inside a chunk
		Vector3i VoxelLayout;

		/// Size and position of each cube that form a voxel.
		const int VoxelCubeSize = 8;
		const Vector3i VoxelCube[8] =
		{
			Vector3i(0, 0, 0),
			Vector3i(1, 0, 0),
			Vector3i(0, 1, 0),
			Vector3i(0, 0, 1),

			Vector3i(1, 0, 1),
			Vector3i(1, 1, 0),
			Vector3i(0, 1, 1),
			Vector3i(1, 1, 1),
		};

		/// Map each position inside a chunk to a index to avoid doing that calculation during
		/// chunk iteration, while building the mesh.
		/// Each cube consists of 8 voxel, each voxel consists of x, y and z position plus the index
		//Vector<int> CubeIndices;

		/// Remember which voxel cube has members that are outside the chunk.
		/// If this is the case we have to access neighboring chunk, which requires extra work.
		//Vector<bool> CubeOutsideChunk;


		SurfaceData(Context* ctx, float voxel_size, Vector3i voxel_layout) :
			Object(ctx),
			Faces(6, 7, -1),
			CubeIndexToFaceLookup(16, 22, STOP),
			Edges(8, 3, 0),
			mVoxelSize(voxel_size),
			VoxelLayout(voxel_layout)
		{
			InitCube();
			InitFaces();
			InitCubeToFaceLookup();
			InitEdges();
			InitPointTable();
		}
	};
}