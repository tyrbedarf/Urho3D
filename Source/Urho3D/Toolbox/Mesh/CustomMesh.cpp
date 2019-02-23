#include "CustomMesh.h"

namespace Urho3D
{
	Vector3 Barycentric(
		const Vector3 &p,
		const Vector3 &a,
		const Vector3 &b,
		const Vector3 &c)
	{
		Vector3 v0 = b - a;
		Vector3 v1 = c - a;
		Vector3 v2 = p - a;
		double d00 = v0.DotProduct(v0);
		double d01 = v0.DotProduct(v1);
		double d11 = v1.DotProduct(v1);
		double d20 = v2.DotProduct(v0);
		double d21 = v2.DotProduct(v1);
		double denom = d00 * d11 - d01 * d01;
		double v = (d11 * d20 - d01 * d21) / denom;
		double w = (d00 * d21 - d01 * d20) / denom;
		double u = 1.0 - v - w;
		return Vector3(u, v, w);
	}

	Vector3 Interpolate(
		const Vector3 &p,
		const Vector3 &a,
		const Vector3 &b,
		const Vector3 &c,
		const Vector3 attrs[3])
	{
		Vector3 bary = Barycentric(p, a, b, c);
		Vector3 out = Vector3(0, 0, 0);
		out = out + attrs[0] * bary.x_;
		out = out + attrs[1] * bary.y_;
		out = out + attrs[2] * bary.z_;
		return out;
	}

	ProceduralMesh::ProceduralMesh(Context* context_) : Object(context_)
	{
		// Prepare Planes to project uv onto surface.
		planes.push_back(Vector3::UP);
		planes.push_back(Vector3::DOWN);
		planes.push_back(Vector3::RIGHT);
		planes.push_back(Vector3::LEFT);
		planes.push_back(Vector3::FORWARD);
		planes.push_back(Vector3::BACK);
	}

	void ProceduralMesh::AddTriangle(Vector3 a, Vector3 b, Vector3 c)
	{
		auto dir1 = b - a;
		auto dir2 = c - a;
		auto normal = dir1.Cross(dir2).Normalized();

		auto uv1 = ProjectVertex(a, normal);
		auto uv2 = ProjectVertex(b, normal);
		auto uv3 = ProjectVertex(c, normal);

		AddTriangle(a, b, c, uv1, uv2, uv3, normal);
	}

	void ProceduralMesh::AddTriangle(
		Vector3 a,
		Vector3 b,
		Vector3 c,
		Vector2 uv1,
		Vector2 uv2,
		Vector2 uv3)
	{
		auto dir1 = b - a;
		auto dir2 = c - a;
		auto normal = dir1.Cross(dir2).Normalized();

		AddTriangle(
			a,
			b,
			c,
			Vector3(uv1.x_, uv1.y_, 0.0f),
			Vector3(uv2.x_, uv2.y_, 0.0f),
			Vector3(uv3.x_, uv3.y_, 0.0f),
			normal);
	}

	void ProceduralMesh::AddTriangle(
		Vector3 a,
		Vector3 b,
		Vector3 c,
		Vector3 uv1,
		Vector3 uv2,
		Vector3 uv3,
		Vector3 normal)
	{
		Triangle t;

		t.v[0] = GetIndex(a);
		t.v[1] = GetIndex(b);
		t.v[2] = GetIndex(c);

		t.n = normal;

		t.uvs[0] = uv1;
		t.uvs[1] = uv2;
		t.uvs[2] = uv3;

		triangles.push_back(t);
	}

	int ProceduralMesh::GetIndex(Vector3 v)
	{
		auto it = VertexIndices.find(v);
		if (it != VertexIndices.end())
		{
			return it->second;
		}

		size_t r = vertices.size();
		Vertex vert;
		vert.p = v;
		vert.tstart = r;
		vertices.push_back(vert);

		VertexIndices.insert(std::pair<Vector3, int>(v, r));

		return r;
	}

	void ProceduralMesh::SetVertex(int index, Vector3 v)
	{
		vertices[index].p = v;
	}

	Model* ProceduralMesh::GetModel()
	{
		return nullptr;
	}

	Geometry* ProceduralMesh::GetGeometry()
	{
		return nullptr;
	}

	Vector3 ProceduralMesh::ProjectVertex(Vector3 point, Vector3 normal)
	{
		int plane = 0;
		float max = std::numeric_limits<float>::lowest();

		for (int i = 0; i < planes.size(); i++)
		{
			float dot = planes[i].Dot(normal);

			if (dot > max)
			{
				plane = i;
				max = dot;
			}

			// Almost coplanar, we are done.
			if (max > 0.95f)
			{
				break;
			}
		}

		Vector3 result = Vector3::ZERO;
		switch (plane)
		{
		case 0:
		{
			result.x_ = point.x_;
			result.y_ = point.z_;

			break;
		}

		case 1:
		{
			result.x_ = point.z_;
			result.y_ = point.x_;

			break;
		}

		case 2:
		{
			result.x_ = point.x_;
			result.y_ = point.y_;

			break;
		}

		case 3:
		{
			result.x_ = point.y_;
			result.y_ = point.x_;

			break;
		}

		case 4:
		{
			result.x_ = point.x_;
			result.y_ = point.y_;

			break;
		}

		case 5:
		{
			result.x_ = point.y_;
			result.y_ = point.x_;

			break;
		}

		default:
		{
			result.x_ = point.x_;
			result.y_ = point.z_;

			break;
		}
		}

		return result;
	}

	void ProceduralMesh::AddTriangleFromIndices(
		int a,
		int b,
		int c,
		bool create_normal,
		bool create_uvs)
	{
		Triangle t;

		t.v[0] = a;
		t.v[1] = b;
		t.v[2] = c;

		auto dir1 = vertices[b].p - vertices[a].p;
		auto dir2 = vertices[c].p - vertices[a].p;

		if (create_normal)
		{
			t.n = dir1.Cross(dir2).Normalized();
		}

		if (create_uvs)
		{
			t.uvs[0] = ProjectVertex(vertices[a].p, t.n);
			t.uvs[1] = ProjectVertex(vertices[b].p, t.n);
			t.uvs[2] = ProjectVertex(vertices[c].p, t.n);
		}

		t.attr = Attributes::NORMAL | Attributes::TEXCOORD;

		triangles.push_back(t);
	}

	void ProceduralMesh::SimplifyMesh(int target_count, double agressiveness, bool verbose)
	{
		// init
		loopi(0, triangles.size())
		{
			triangles[i].deleted = 0;
		}

		// main iteration loop
		int deleted_triangles = 0;
		std::vector<int> deleted0, deleted1;
		int triangle_count = triangles.size();

		for (int iteration = 0; iteration < 100; iteration++)
		{
			if (triangle_count - deleted_triangles <= target_count) break;

			// update mesh once in a while
			if (iteration % 5 == 0)
			{
				UpdateMesh(iteration);
			}

			// clear dirty flag
			loopi(0, triangles.size()) triangles[i].dirty = 0;

			//
			// All triangles with edges below the threshold will be removed
			//
			// The following numbers works well for most models.
			// If it does not, try to adjust the 3 parameters
			//
			double threshold = 0.000000001*pow(double(iteration + 3), agressiveness);

			// target number of triangles reached ? Then break
			if ((verbose) && (iteration % 5 == 0))
			{
				printf("iteration %d - triangles %d threshold %g\n",
					iteration,
					triangle_count - deleted_triangles,
					threshold);
			}

			// remove vertices & mark deleted triangles
			loopi(0, triangles.size())
			{
				Triangle &t = triangles[i];
				if (t.err[3] > threshold) continue;
				if (t.deleted) continue;
				if (t.dirty) continue;

				loopj(0, 3)if (t.err[j] < threshold)
				{

					int i0 = t.v[j]; Vertex &v0 = vertices[i0];
					int i1 = t.v[(j + 1) % 3]; Vertex &v1 = vertices[i1];
					// Border check
					if (v0.border != v1.border)  continue;

					// Compute vertex to collapse to
					Vector3 p;
					CalculateError(i0, i1, p);
					deleted0.resize(v0.tcount); // normals temporarily
					deleted1.resize(v1.tcount); // normals temporarily
												// don't remove if flipped
					if (Flipped(p, i0, i1, v0, v1, deleted0)) continue;

					if (Flipped(p, i1, i0, v1, v0, deleted1)) continue;

					if ((t.attr & TEXCOORD) == TEXCOORD)
					{
						UpdateUvs(i0, v0, p, deleted0);
						UpdateUvs(i0, v1, p, deleted1);
					}

					// not flipped, so remove edge
					v0.p = p;
					v0.q = v1.q + v0.q;
					int tstart = refs.size();

					UpdateTriangles(i0, v0, deleted0, deleted_triangles);
					UpdateTriangles(i0, v1, deleted1, deleted_triangles);

					int tcount = refs.size() - tstart;

					if (tcount <= v0.tcount)
					{
						// save ram
						if (tcount)memcpy(&refs[v0.tstart], &refs[tstart], tcount * sizeof(VTRef));
					}
					else
						// append
						v0.tstart = tstart;

					v0.tcount = tcount;
					break;
				}

				// done?
				if (triangle_count - deleted_triangles <= target_count) break;
			}
		}
		// clean up mesh
		CompactMesh();
	}

	void ProceduralMesh::SimplifyMeshLossless(bool verbose)
	{
		// init
		printf("Before: Vertices %zd Triangles: %zd\n", vertices.size(), triangles.size());
		loopi(0, triangles.size()) triangles[i].deleted = 0;

		// main iteration loop
		int deleted_triangles = 0;
		std::vector<int> deleted0, deleted1;
		int triangle_count = triangles.size();
		//int iteration = 0;
		//loop(iteration,0,100)
		for (int iteration = 0; iteration < 9999; iteration++)
		{
			// update mesh constantly
			UpdateMesh(iteration);

			// clear dirty flag
			loopi(0, triangles.size()) triangles[i].dirty = 0;

			//
			// All triangles with edges below the threshold will be removed
			//
			// The following numbers works well for most models.
			// If it does not, try to adjust the 3 parameters
			//
			double threshold = DBL_EPSILON; //1.0E-3 EPS;
			if (verbose)
			{
				printf("lossless iteration %d\n", iteration);
			}

			// remove vertices & mark deleted triangles
			loopi(0, triangles.size())
			{
				Triangle &t = triangles[i];
				if (t.err[3] > threshold) continue;
				if (t.deleted) continue;
				if (t.dirty) continue;

				loopj(0, 3)if (t.err[j] < threshold)
				{
					int i0 = t.v[j]; Vertex &v0 = vertices[i0];
					int i1 = t.v[(j + 1) % 3]; Vertex &v1 = vertices[i1];

					// Border check
					if (v0.border != v1.border)  continue;

					// Compute vertex to collapse to
					Vector3 p;
					CalculateError(i0, i1, p);

					deleted0.resize(v0.tcount); // normals temporarily
					deleted1.resize(v1.tcount); // normals temporarily

												// don't remove if flipped
					if (Flipped(p, i0, i1, v0, v1, deleted0)) continue;
					if (Flipped(p, i1, i0, v1, v0, deleted1)) continue;

					if ((t.attr & TEXCOORD) == TEXCOORD)
					{
						UpdateUvs(i0, v0, p, deleted0);
						UpdateUvs(i0, v1, p, deleted1);
					}

					// not flipped, so remove edge
					v0.p = p;
					v0.q = v1.q + v0.q;
					int tstart = refs.size();

					UpdateTriangles(i0, v0, deleted0, deleted_triangles);
					UpdateTriangles(i0, v1, deleted1, deleted_triangles);

					int tcount = refs.size() - tstart;

					if (tcount <= v0.tcount)
					{
						// save ram
						if (tcount)memcpy(&refs[v0.tstart], &refs[tstart], tcount * sizeof(VTRef));
					}
					else
						// append
						v0.tstart = tstart;

					v0.tcount = tcount;
					break;
				}
			}

			if (deleted_triangles <= 0) break;
			deleted_triangles = 0;
		}

		// clean up mesh
		CompactMesh();
	}

	bool ProceduralMesh::Flipped(
		Vector3 p,
		int i0,
		int i1,
		Vertex &v0,
		Vertex &v1,
		std::vector<int> &deleted)
	{
		loopk(0, v0.tcount)
		{
			Triangle &t = triangles[refs[v0.tstart + k].tid];
			if (t.deleted) continue;

			int s = refs[v0.tstart + k].tvertex;
			int id1 = t.v[(s + 1) % 3];
			int id2 = t.v[(s + 2) % 3];

			if (id1 == i1 || id2 == i1) // delete ?
			{

				deleted[k] = 1;
				continue;
			}
			Vector3 d1 = vertices[id1].p - p; d1.Normalize();
			Vector3 d2 = vertices[id2].p - p; d2.Normalize();
			if (fabs(d1.DotProduct(d2)) > 0.999) return true;

			Vector3 n;
			// n.cross(d1, d2);
			n = d1.CrossProduct(d2);
			n.Normalize();
			deleted[k] = 0;
			if (n.DotProduct(t.n) < 0.2) return true;
		}

		return false;
	}

	void ProceduralMesh::UpdateUvs(
		int i0,
		const Vertex &v,
		const Vector3 &p,
		std::vector<int> &deleted)
	{
		loopk(0, v.tcount)
		{
			VTRef &r = refs[v.tstart + k];
			Triangle &t = triangles[r.tid];
			if (t.deleted)continue;
			if (deleted[k])continue;
			Vector3 p1 = vertices[t.v[0]].p;
			Vector3 p2 = vertices[t.v[1]].p;
			Vector3 p3 = vertices[t.v[2]].p;
			t.uvs[r.tvertex] = Interpolate(p, p1, p2, p3, t.uvs);
		}
	}

	void ProceduralMesh::UpdateTriangles(
		int i0,
		Vertex &v,
		std::vector<int> &deleted,
		int &deleted_triangles)
	{
		Vector3 p;
		loopk(0, v.tcount)
		{
			VTRef &r = refs[v.tstart + k];
			Triangle &t = triangles[r.tid];
			if (t.deleted)continue;
			if (deleted[k])
			{
				t.deleted = 1;
				deleted_triangles++;
				continue;
			}
			t.v[r.tvertex] = i0;
			t.dirty = 1;
			t.err[0] = CalculateError(t.v[0], t.v[1], p);
			t.err[1] = CalculateError(t.v[1], t.v[2], p);
			t.err[2] = CalculateError(t.v[2], t.v[0], p);
			t.err[3] = fmin(t.err[0], fmin(t.err[1], t.err[2]));
			refs.push_back(r);
		}
	}

	void ProceduralMesh::UpdateMesh(int iteration)
	{
		if (iteration > 0) // compact triangles
		{
			int dst = 0;
			loopi(0, triangles.size())
				if (!triangles[i].deleted)
				{
					triangles[dst++] = triangles[i];
				}
			triangles.resize(dst);
		}

		//
		// Init Quadrics by Plane & Edge Errors
		//
		// required at the beginning ( iteration == 0 )
		// recomputing during the simplification is not required,
		// but mostly improves the result for closed meshes
		//
		if (iteration == 0)
		{
			loopi(0, vertices.size())
				vertices[i].q = SymetricMatrix(0.0);

			loopi(0, triangles.size())
			{
				Triangle &t = triangles[i];
				Vector3 n, p[3];
				loopj(0, 3) p[j] = vertices[t.v[j]].p;
				// n.cross(p[1] - p[0], p[2] - p[0]);
				n = (p[1] - p[0]).CrossProduct(p[2] - p[0]);
				n.Normalize();
				t.n = n;
				loopj(0, 3) vertices[t.v[j]].q =
					vertices[t.v[j]].q + SymetricMatrix(n.x_, n.y_, n.z_, -n.DotProduct(p[0]));
			}
			loopi(0, triangles.size())
			{
				// Calc Edge Error
				Triangle &t = triangles[i]; Vector3 p;
				loopj(0, 3) t.err[j] = CalculateError(t.v[j], t.v[(j + 1) % 3], p);
				t.err[3] = fmin(t.err[0], fmin(t.err[1], t.err[2]));
			}
		}

		// Init Reference ID list
		loopi(0, vertices.size())
		{
			vertices[i].tstart = 0;
			vertices[i].tcount = 0;
		}

		loopi(0, triangles.size())
		{
			Triangle &t = triangles[i];
			loopj(0, 3) vertices[t.v[j]].tcount++;
		}

		int tstart = 0;
		loopi(0, vertices.size())
		{
			Vertex &v = vertices[i];
			v.tstart = tstart;
			tstart += v.tcount;
			v.tcount = 0;
		}

		// Write References
		refs.resize(triangles.size() * 3);
		loopi(0, triangles.size())
		{
			Triangle &t = triangles[i];
			loopj(0, 3)
			{
				Vertex &v = vertices[t.v[j]];
				refs[v.tstart + v.tcount].tid = i;
				refs[v.tstart + v.tcount].tvertex = j;
				v.tcount++;
			}
		}

		// Identify boundary : vertices[].border=0,1
		if (iteration == 0)
		{
			std::vector<int> vcount, vids;

			loopi(0, vertices.size())
				vertices[i].border = 0;

			loopi(0, vertices.size())
			{
				Vertex &v = vertices[i];
				vcount.clear();
				vids.clear();
				loopj(0, v.tcount)
				{
					int k = refs[v.tstart + j].tid;
					Triangle &t = triangles[k];
					loopk(0, 3)
					{
						int ofs = 0, id = t.v[k];
						while (ofs < vcount.size())
						{
							if (vids[ofs] == id)break;
							ofs++;
						}
						if (ofs == vcount.size())
						{
							vcount.push_back(1);
							vids.push_back(id);
						}
						else
							vcount[ofs]++;
					}
				}

				loopj(0, vcount.size()) if (vcount[j] == 1)
					vertices[vids[j]].border = 1;
			}
		}
	}

	void ProceduralMesh::CompactMesh()
	{
		int dst = 0;
		loopi(0, vertices.size())
		{
			vertices[i].tcount = 0;
		}

		loopi(0, triangles.size())
			if (!triangles[i].deleted)
			{
				Triangle &t = triangles[i];
				triangles[dst++] = t;
				loopj(0, 3) vertices[t.v[j]].tcount = 1;
			}

		triangles.resize(dst);
		dst = 0;
		loopi(0, vertices.size())
			if (vertices[i].tcount)
			{
				vertices[i].tstart = dst;
				vertices[dst].p = vertices[i].p;
				dst++;
			}

		loopi(0, triangles.size())
		{
			Triangle &t = triangles[i];
			loopj(0, 3)t.v[j] = vertices[t.v[j]].tstart;
		}

		vertices.resize(dst);
	}

	double ProceduralMesh::VertexError(SymetricMatrix q, double x, double y, double z)
	{
		return   q[0] * x*x + 2 * q[1] * x*y + 2 * q[2] * x*z + 2 * q[3] * x + q[4] * y*y
			+ 2 * q[5] * y*z + 2 * q[6] * y + q[7] * z*z + 2 * q[8] * z + q[9];
	}

	double ProceduralMesh::CalculateError(int id_v1, int id_v2, Vector3 &p_result)
	{
		// compute interpolated vertex
		SymetricMatrix q = vertices[id_v1].q + vertices[id_v2].q;
		bool   border = vertices[id_v1].border & vertices[id_v2].border;
		double error = 0;
		double det = q.det(0, 1, 2, 1, 4, 5, 2, 5, 7);
		if (det != 0 && !border)
		{
			// q_delta is invertible
			p_result.x_ = -1 / det * (q.det(1, 2, 3, 4, 5, 6, 5, 7, 8));
			p_result.y_ = 1 / det * (q.det(0, 2, 3, 1, 5, 6, 2, 7, 8));
			p_result.z_ = -1 / det * (q.det(0, 1, 3, 1, 4, 6, 2, 5, 8));

			error = VertexError(q, p_result.x_, p_result.y_, p_result.z_);
		}
		else
		{
			// det = 0 -> try to find best result
			Vector3 p1 = vertices[id_v1].p;
			Vector3 p2 = vertices[id_v2].p;
			Vector3 p3 = (p1 + p2) / 2;
			double error1 = VertexError(q, p1.x_, p1.y_, p1.z_);
			double error2 = VertexError(q, p2.x_, p2.y_, p2.z_);
			double error3 = VertexError(q, p3.x_, p3.y_, p3.z_);
			error = fmin(error1, fmin(error2, error3));
			if (error1 == error) p_result = p1;
			if (error2 == error) p_result = p2;
			if (error3 == error) p_result = p3;
		}

		return error;
	}
}
