/*
* Adopted from https://github.com/sp4cerat/Fast-Quadric-Mesh-Simplification
*
* Mesh Simplification (C)2014 by Sven Forstmann in 2014, MIT License
*
*/
#ifndef CUSTOM_MESH_H
#define CUSTOM_MESH_H

#include <unordered_map>
#include <math.h>
#include <vector>
#include <map>

#include "../../Math/Vector3.h"
#include "../../Core/Object.h"
#include "../../Graphics/Model.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/VertexBuffer.h"
#include "../../Graphics/IndexBuffer.h"

#include "SymetricMatrix.h"

namespace Urho3D
{
	enum Attributes {
		NONE,
		NORMAL		= 2,
		TEXCOORD	= 4,
		COLOR		= 8
	};

	struct Triangle
	{
		int v[3];
		double err[4];
		int deleted;
		int dirty;
		Vector3 n;
		Vector3 uvs[3];
		int attr;
	};

	struct Vertex
	{
		Vector3 p;
		int tstart;
		int tcount;
		SymetricMatrix q;
		int border;
	};

	struct VTRef
	{
		int tid;
		int tvertex;
	};

	class ProceduralMesh : public Object
	{
		URHO3D_OBJECT(ProceduralMesh, Object)

	public:
		/// C-Tor
		ProceduralMesh(Context* context_);

		/// Create a new triangle using the given positions. Will create
		/// uv (by projection) and normal for the triangle.
		void AddTriangle(Vector3 a, Vector3 b, Vector3 c);

		/// Create a new triangle using the given positions.
		/// Will create a normal as well.
		void AddTriangle(
			Vector3 a,
			Vector3 b,
			Vector3 c,
			Vector2 uv1,
			Vector2 uv2,
			Vector2 uv3);

		/// Create a new Triangle. Only one normal
		/// since this version of CustomMesh is build for mesh simplification.
		void AddTriangle(
			Vector3 a,
			Vector3 b,
			Vector3 c,
			Vector3 uv1,
			Vector3 uv2,
			Vector3 uv3,
			Vector3 normal);

		/// Create a triangle using the given indices into the vertex array.
		void AddTriangleFromIndices(
			int a,
			int b,
			int c,
			bool create_normal = false,
			bool create_uvs = false);

		SharedPtr<Model> GetModel();

		/// Main simplification function
		///
		/// \param target_count target nr. of triangles
		/// \param agressiveness sharpness to increase the threshold. 5..8 are good numbers more iterations yield higher quality
		/// \param verbose print extra debug information to the console.
		void SimplifyMesh(int target_count, double agressiveness = 7, bool verbose = false);

		/// Same as above, the target count is expressed as percentage of current
		/// triangle count.
		///
		/// \param target_count_percentage must be between 0.0 and 1.0.
		/// \param agressiveness sharpness to increase the threshold. 5..8 are good numbers more iterations yield higher quality
		/// \param verbose print extra debug information to the console.
		void SimplifyMesh(float target_count_percentage, double agressiveness = 7, bool verbose = false);

		/// Collapses coplanar faces only.
		void SimplifyMeshLossless(bool verbose = false);

		/// Get or create a new vertex.
		int GetIndex(Vector3 v);

		/// Set the position of a vertex.
		/// Note: vertex_indices is not beeing updated.
		/// This allows to modify existing vertices, after they have been created.
		/// This makes it easier (and a bit more performant in fact) to implement
		/// algorithmes that extract implicit surfaces (i. e. voxels) like
		/// 'Surface Nets'.
		/// (Careful, no bounds check!)
		void SetVertex(int index, Vector3 v);

		/// Clear mesh data.
		void Clear()
		{
			triangles.clear();
			vertices.clear();
			refs.clear();
			VertexIndices.clear();
		}

	private:
		// Todo: Change to pointers?
		std::vector<Triangle> triangles;
		std::vector<Vertex> vertices;
		std::vector<VTRef> refs;

		std::vector<Vector3> planes;

		/// Note: The key is the position a vertex had, when it
		/// was first inserted. The actual position might change.
		/// (using get_index and set_vertex for example)
		std::map<Vector3, size_t> VertexIndices;

		/// Check if a triangle flips when this edge is removed
		bool Flipped(Vector3 p, int i0, int i1, Vertex &v0, Vertex &v1, std::vector<int> &deleted);

		void UpdateUvs(int i0, const Vertex &v, const Vector3 &p, std::vector<int> &deleted);

		/// Update triangle connections and edge error after a edge is collapsed
		void UpdateTriangles(int i0, Vertex &v, std::vector<int> &deleted, int &deleted_triangles);

		/// compact triangles, compute edge error and build reference list
		void UpdateMesh(int iteration);

		/// Finally compact mesh before exiting
		void CompactMesh();

		/// Error between vertex and Quadric
		double VertexError(SymetricMatrix q, double x, double y, double z);

		/// Error for one edge
		double CalculateError(int id_v1, int id_v2, Vector3 &p_result);

		/// Pick components of the point depending where the normal is pointing.
		Vector3 ProjectVertex(Vector3 point, Vector3 normal);
	};
}
#endif
