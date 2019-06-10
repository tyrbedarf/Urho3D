#include "ProceduralMesh.h"

#include "../../IO/Log.h"
#include "../../Resource/ResourceCache.h"
#include "../../Graphics/GraphicsDefs.h"

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

	Vector3 calcNormal(Vector3 a, Vector3 b, Vector3 c)
	{
		auto dir1 = b - a;
		auto dir2 = c - a;

		return dir1.Cross(dir2).Normalized();
	}

	ProceduralMesh::ProceduralMesh(Context* context_) : Object(context_)
	{
		// Prepare Planes to project uv onto surface.
		planes.Push(Vector3::UP);
		planes.Push(Vector3::DOWN);
		planes.Push(Vector3::RIGHT);
		planes.Push(Vector3::LEFT);
		planes.Push(Vector3::FORWARD);
		planes.Push(Vector3::BACK);
	}

	void ProceduralMesh::AddTriangle(Vector3 a, Vector3 b, Vector3 c)
	{
		auto normal = calcNormal(a, b, c);

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
		auto normal = calcNormal(a, b, c);

		AddTriangle(
			a,
			b,
			c,
			uv1,
			uv2,
			uv3,
			normal);
	}

	void ProceduralMesh::AddTriangle(
		Vector3 a,
		Vector3 b,
		Vector3 c,
		Vector2 uv1,
		Vector2 uv2,
		Vector2 uv3,
		Vector3 normal)
	{
		Triangle t;

		t.v[0] = GetIndex(a);
		t.v[1] = GetIndex(b);
		t.v[2] = GetIndex(c);

		t.n = normal;

		t.uvs[0] = Vector3(uv1.x_, uv1.y_);
		t.uvs[1] = Vector3(uv2.x_, uv2.y_);
		t.uvs[2] = Vector3(uv3.x_, uv3.y_);

		triangles.Push(t);
	}

	void ProceduralMesh::FromModel(Model* model, unsigned int index, unsigned int lod, bool verbose)
	{
		if (!model)
		{
			URHO3D_LOGERROR("Model not set.");
			return;
		}

		FromGeometry(model->GetGeometry(index, lod), index, verbose);
	}

	void ProceduralMesh::FromGeometry(Geometry* geom, unsigned int index, bool verbose)
	{
		if (!geom)
		{
			URHO3D_LOGERROR("Geometry not set.");
			return;
		}

		VertexBuffer* vertexBuffer = geom->GetVertexBuffer(index);
		IndexBuffer* indexBuffer = geom->GetIndexBuffer();

		const auto* vertexData =
			(const unsigned char*)
			vertexBuffer->Lock(0, vertexBuffer->GetVertexCount());

		const auto* indexData =
			(const unsigned char*)
			indexBuffer->Lock(0, indexBuffer->GetIndexCount());

		if (!vertexData)
		{
			URHO3D_LOGERROR("Could not lock vertex buffer.");
			return;
		}

		if (!indexData)
		{
			URHO3D_LOGERROR("Could not lock index buffer.");
			return;
		}

		int numVertices = vertexBuffer->GetVertexCount();
		int numIndices = indexBuffer->GetIndexCount();
		int vertexSize = vertexBuffer->GetVertexSize();
		int indexSize = indexBuffer->GetIndexSize();

		if (verbose)
		{
			URHO3D_LOGDEBUGF("Idx Size: %d - Indices : %d", indexSize, numIndices);
			URHO3D_LOGDEBUGF("Vtx Size: %d - Vertices: %d", vertexSize, numVertices);
		}

		for (int i = 0; i < numIndices; i += 3)
		{
			int offset_a = 0;
			int offset_b = indexSize;
			int offset_c = indexSize + indexSize;

			unsigned short a = *reinterpret_cast<const unsigned short*>(indexData + (i * indexSize) + offset_a);
			unsigned short b = *reinterpret_cast<const unsigned short*>(indexData + (i * indexSize) + offset_b);
			unsigned short c = *reinterpret_cast<const unsigned short*>(indexData + (i * indexSize) + offset_c);

			Vector3 va = *reinterpret_cast<const Vector3*>(vertexData + (a * vertexSize));
			Vector3 vb = *reinterpret_cast<const Vector3*>(vertexData + (b * vertexSize));
			Vector3 vc = *reinterpret_cast<const Vector3*>(vertexData + (c * vertexSize));

			AddTriangle(va, vb, vc);
		}
	}

	void ProceduralMesh::FromFile(String ressource, unsigned int index, unsigned int lod, bool verbose)
	{
		auto* cache = GetSubsystem<ResourceCache>();
		Model* model = cache->GetResource<Model>(ressource);
		if (!model)
		{
			URHO3D_LOGERROR("Model could not be loaded");
			return;
		}

		FromModel(model, index, lod, verbose);
	}

	int ProceduralMesh::GetIndex(Vector3 v)
	{
		auto it = VertexIndices.Find(v);
		if (it != VertexIndices.End())
		{
			assert(it->second_ < vertices.Size());
			return it->second_;
		}

		size_t r = vertices.Size();
		Vertex vert;
		vert.p = v;
		vert.tstart = r;
		vertices.Push(vert);

		VertexIndices.Insert(Pair<Vector3, size_t>(v, r));

		return r;
	}

	void ProceduralMesh::SetVertex(int index, Vector3 v)
	{
		vertices[index].p = v;
	}

	SharedPtr<Model> ProceduralMesh::GetModel()
	{
		SharedPtr<Model> model		= SharedPtr<Model>(new Model(context_));
		SharedPtr<VertexBuffer> vb	= SharedPtr<VertexBuffer>(new VertexBuffer(context_));
		SharedPtr<IndexBuffer> ib	= SharedPtr<IndexBuffer>(new IndexBuffer(context_));
		SharedPtr<Geometry> geom	= SharedPtr<Geometry>(new Geometry(context_));
		BoundingBox boundingBox;

		PODVector<VertexElement> elements;
		elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
		elements.Push(VertexElement(TYPE_VECTOR3, SEM_NORMAL));
		elements.Push(VertexElement(TYPE_VECTOR2, SEM_TEXCOORD));
		elements.Push(VertexElement(TYPE_VECTOR4, SEM_TANGENT));

		const int vertexSize = 12;
		PODVector<float> vertexData; // (vertices.Size() * vertexSize);
		PODVector<unsigned short> indexData;

		for (int i = 0; i < triangles.Size(); i++)
		{
			Triangle t = triangles[i];
			if (t.deleted)
			{
				continue;
			}

			Vector3 a = vertices[t.v[0]].p;
			Vector3 b = vertices[t.v[1]].p;
			Vector3 c = vertices[t.v[2]].p;

			auto normal = calcNormal(a, b, c);
			auto uv1 = ProjectVertex(a, normal);
			auto uv2 = ProjectVertex(b, normal);
			auto uv3 = ProjectVertex(c, normal);

			boundingBox.Merge(a);
			boundingBox.Merge(b);
			boundingBox.Merge(c);

			int ia = (vertexData.Size() / vertexSize) + 0;
			int ib = (vertexData.Size() / vertexSize) + 1;
			int ic = (vertexData.Size() / vertexSize) + 2;

			indexData.Push(ia);
			indexData.Push(ib);
			indexData.Push(ic);

			/// Vertex 1
			vertexData.Push(a.x_);
			vertexData.Push(a.y_);
			vertexData.Push(a.z_);

			vertexData.Push(normal.x_);
			vertexData.Push(normal.y_);
			vertexData.Push(normal.z_);

			vertexData.Push(uv1.x_);
			vertexData.Push(uv1.y_);

			vertexData.Push(0.0f);
			vertexData.Push(0.0f);
			vertexData.Push(0.0f);
			vertexData.Push(0.0f);

			/// Vertex 2
			vertexData.Push(b.x_);
			vertexData.Push(b.y_);
			vertexData.Push(b.z_);

			vertexData.Push(normal.x_);
			vertexData.Push(normal.y_);
			vertexData.Push(normal.z_);

			vertexData.Push(uv2.x_);
			vertexData.Push(uv2.y_);

			vertexData.Push(0.0f);
			vertexData.Push(0.0f);
			vertexData.Push(0.0f);
			vertexData.Push(0.0f);

			/// Vertex 3
			vertexData.Push(c.x_);
			vertexData.Push(c.y_);
			vertexData.Push(c.z_);

			vertexData.Push(normal.x_);
			vertexData.Push(normal.y_);
			vertexData.Push(normal.z_);

			vertexData.Push(uv3.x_);
			vertexData.Push(uv3.y_);

			vertexData.Push(0.0f);
			vertexData.Push(0.0f);
			vertexData.Push(0.0f);
			vertexData.Push(0.0f);
		}

		if (vertices.Size() < 1 || indexData.Size() < 1)
		{
			return nullptr;
		}

		/// Calculate Tangents
		Vector<Vector3> tan1(indexData.Size(), Vector3(0.0f, 0.0f, 0.0f));
		Vector<Vector3> tan2(indexData.Size(), Vector3(0.0f, 0.0f, 0.0f));
		for (int i = 0; i < indexData.Size(); i += 3)
		{
			int i1 = indexData[i + 0];
			int i2 = indexData[i + 1];
			int i3 = indexData[i + 2];

			Vector3 v1(vertexData[(i1 * vertexSize) + 0], vertexData[(i1 * vertexSize) + 1], vertexData[(i1 * vertexSize) + 2]);
			Vector3 v2(vertexData[(i2 * vertexSize) + 0], vertexData[(i2 * vertexSize) + 1], vertexData[(i2 * vertexSize) + 2]);
			Vector3 v3(vertexData[(i3 * vertexSize) + 0], vertexData[(i3 * vertexSize) + 1], vertexData[(i3 * vertexSize) + 2]);

			Vector2 w1(vertexData[(i1 * vertexSize) + 6], vertexData[(i1 * vertexSize) + 7]);
			Vector2 w2(vertexData[(i2 * vertexSize) + 6], vertexData[(i2 * vertexSize) + 7]);
			Vector2 w3(vertexData[(i3 * vertexSize) + 6], vertexData[(i3 * vertexSize) + 7]);

			float x1 = v2.x_ - v1.x_;
			float x2 = v3.x_ - v1.x_;
			float y1 = v2.y_ - v1.y_;
			float y2 = v3.y_ - v1.y_;
			float z1 = v2.z_ - v1.z_;
			float z2 = v3.z_ - v1.z_;
			float s1 = w2.x_ - w1.x_;
			float s2 = w3.x_ - w1.x_;
			float t1 = w2.y_ - w1.y_;
			float t2 = w3.y_ - w1.y_;
			float r = 1.0f / (s1 * t2 - s2 * t1);

			Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
			Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;

			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}

		for (long i = 0; i < vertexData.Size(); i += vertexSize)
		{
			Vector3 n(vertexData[i + 3], vertexData[i + 4], vertexData[i + 5]);
			Vector3 t(tan1[i / vertexSize].x_, tan1[i / vertexSize].y_, tan1[i / vertexSize].z_);

			// Gram-Schmidt orthogonalize
			auto ortho = (t - n * n.DotProduct(t)).Normalized();
			vertexData[i + 8] = ortho.x_;
			vertexData[i + 9] = ortho.y_;
			vertexData[i + 10] = ortho.z_;

			// Calculate handedness
			vertexData[i + 11] = (n.CrossProduct(t).DotProduct(tan2[i / vertexSize]) < 0.0F) ? -1.0F : 1.0F;
		}

		vb->SetSize(vertexData.Size() / vertexSize, elements);
		vb->SetData(vertexData.Buffer());

		ib->SetSize(indexData.Size(), false);
		ib->SetData(indexData.Buffer());

		ib->SetShadowed(true);
		vb->SetShadowed(true);

		geom->SetNumVertexBuffers(1);
		geom->SetVertexBuffer(0, vb);
		geom->SetIndexBuffer(ib);
		geom->SetDrawRange(TRIANGLE_LIST, 0, indexData.Size());

		model->SetNumGeometries(1);
		model->SetGeometry(0, 0, geom);
		model->SetBoundingBox(boundingBox);

		Vector<SharedPtr<VertexBuffer>> vertexBuffers;
		Vector<SharedPtr<IndexBuffer>> indexBuffers;
		vertexBuffers.Push(vb);
		indexBuffers.Push(ib);

		PODVector<unsigned> morphRangeStarts;
		PODVector<unsigned> morphRangeCounts;
		morphRangeStarts.Push(0);
		morphRangeCounts.Push(0);

		model->SetVertexBuffers(vertexBuffers, morphRangeStarts, morphRangeCounts);
		model->SetIndexBuffers(indexBuffers);

		return model;
	}

	Vector2 ProceduralMesh::ProjectVertex(Vector3 point, Vector3 normal)
	{
		int plane = 0;
		float max = std::numeric_limits<float>::lowest();

		for (int i = 0; i < planes.Size(); i++)
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

		Vector2 result = Vector2::ZERO;
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
			result.x_ = point.z_;
			result.y_ = point.y_;

			break;
		}

		case 3:
		{
			result.x_ = point.y_;
			result.y_ = point.z_;

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

		if (create_normal)
		{
			t.n = calcNormal(vertices[a].p, vertices[b].p, vertices[c].p);
		}

		/*if (create_uvs)
		{
			t.uvs[0] = ProjectVertex(vertices[a].p, t.n);
			t.uvs[1] = ProjectVertex(vertices[b].p, t.n);
			t.uvs[2] = ProjectVertex(vertices[c].p, t.n);
		}*/

		t.attr = Attributes::NORMAL | Attributes::TEXCOORD;

		triangles.Push(t);
	}
	void ProceduralMesh::SimplifyMesh(
		float target_count_percentage,
		double agressiveness,
		bool verbose)
	{
		int target_count = (int) (vertices.Size() * target_count_percentage);
		SimplifyMesh(target_count, agressiveness, verbose);
	}

	void ProceduralMesh::SimplifyMesh(int target_count, double agressiveness, bool verbose)
	{
		// init
		loopi(0, triangles.Size())
		{
			triangles[i].deleted = 0;
		}

		// main iteration loop
		int deleted_triangles = 0;
		std::vector<int> deleted0, deleted1;
		int triangle_count = triangles.Size();

		for (int iteration = 0; iteration < 100; iteration++)
		{
			if (triangle_count - deleted_triangles <= target_count) break;

			// update mesh once in a while
			if (iteration % 5 == 0)
			{
				UpdateMesh(iteration);
			}

			// clear dirty flag
			loopi(0, triangles.Size()) triangles[i].dirty = 0;

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
				URHO3D_LOGDEBUGF("iteration %d - triangles %d threshold %g\n",
					iteration,
					triangle_count - deleted_triangles,
					threshold);
			}

			// remove vertices & mark deleted triangles
			loopi(0, triangles.Size())
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
					int tstart = refs.Size();

					UpdateTriangles(i0, v0, deleted0, deleted_triangles);
					UpdateTriangles(i0, v1, deleted1, deleted_triangles);

					int tcount = refs.Size() - tstart;

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

	void ProceduralMesh::SimplifyMeshLossless(bool verbose, int maxIterations)
	{
		// init
		if (verbose)
		{
			URHO3D_LOGDEBUGF("Before. Vertices: %d Triangles: %d", vertices.Size(), triangles.Size());
		}

		loopi(0, triangles.Size()) triangles[i].deleted = 0;

		// main iteration loop
		int deleted_triangles = 0;
		std::vector<int> deleted0, deleted1;
		int triangle_count = triangles.Size();

		for (int iteration = 0; iteration < maxIterations; iteration++)
		{
			// update mesh constantly
			UpdateMesh(iteration);

			// clear dirty flag
			loopi(0, triangles.Size()) triangles[i].dirty = 0;

			// All triangles with edges below the threshold will be removed
			//
			// The following numbers works well for most models.
			// If it does not, try to adjust the 3 parameters
			double threshold = DBL_EPSILON; //1.0E-3 EPS;
			if (verbose)
			{
				URHO3D_LOGDEBUGF("lossless iteration %d", iteration);
			}

			// remove vertices & mark deleted triangles
			loopi(0, triangles.Size())
			{
				Triangle &t = triangles[i];
				if (t.err[3] > threshold) continue;
				if (t.deleted) continue;
				if (t.dirty) continue;

				loopj(0, 3)if (t.err[j] < threshold)
				{
					int i0 = t.v[j];
					Vertex &v0 = vertices[i0];
					int i1 = t.v[(j + 1) % 3];
					Vertex &v1 = vertices[i1];

					// Border check -> Does not seem to work borders are beeing
					// identified correctly but vertices are beeing collapsed
					// wrong. Just leave borders alone.
					// if (v0.border != v1.border)  continue;
					if (v0.border > 0 || v1.border > 0)
					{

						continue;
					}

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
					int tstart = refs.Size();

					UpdateTriangles(i0, v0, deleted0, deleted_triangles);
					UpdateTriangles(i0, v1, deleted1, deleted_triangles);

					int tcount = refs.Size() - tstart;

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

		if (verbose)
		{
			URHO3D_LOGDEBUGF("After. Vertices: %d Triangles: %d", vertices.Size(), triangles.Size());
		}
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
			refs.Push(r);
		}
	}

	void ProceduralMesh::UpdateMesh(int iteration)
	{
		if (iteration > 0) // compact triangles
		{
			int dst = 0;
			loopi(0, triangles.Size())
				if (!triangles[i].deleted)
				{
					triangles[dst++] = triangles[i];
				}
			triangles.Resize(dst);
		}

		// Init Reference ID list
		loopi(0, vertices.Size())
		{
			vertices[i].tstart = 0;
			vertices[i].tcount = 0;
		}

		loopi(0, triangles.Size())
		{
			Triangle &t = triangles[i];
			loopj(0, 3) vertices[t.v[j]].tcount++;
		}

		int tstart = 0;
		loopi(0, vertices.Size())
		{
			Vertex &v = vertices[i];
			v.tstart = tstart;
			tstart += v.tcount;
			v.tcount = 0;
		}

		// Write References
		refs.Resize(triangles.Size() * 3);
		loopi(0, triangles.Size())
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
			Vector<int> vcount, vids;

			loopi(0, vertices.Size())
				vertices[i].border = 0;

			loopi(0, vertices.Size())
			{
				Vertex &v = vertices[i];
				vcount.Clear();
				vids.Clear();
				loopj(0, v.tcount)
				{
					int k = refs[v.tstart + j].tid;
					Triangle &t = triangles[k];
					loopk(0, 3)
					{
						int ofs = 0, id = t.v[k];
						while (ofs < vcount.Size())
						{
							if (vids[ofs] == id)break;
							ofs++;
						}
						if (ofs == vcount.Size())
						{
							vcount.Push(1);
							vids.Push(id);
						}
						else
							vcount[ofs]++;
					}
				}

				loopj(0, vcount.Size()) if (vcount[j] == 1)
					vertices[vids[j]].border = 1;
			}
		}

		// Init Quadrics by Plane & Edge Errors
		//
		// required at the beginning ( iteration == 0 )
		// recomputing during the simplification is not required,
		// but mostly improves the result for closed meshes
		if (iteration == 0)
		{
			loopi(0, vertices.Size())
				vertices[i].q = SymetricMatrix(0.0);

			loopi(0, triangles.Size())
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

			loopi(0, triangles.Size())
			{
				// Calc Edge Error
				Triangle &t = triangles[i];
				Vector3 p;
				loopj(0, 3) t.err[j] = CalculateError(t.v[j], t.v[(j + 1) % 3], p);
				t.err[3] = fmin(t.err[0], fmin(t.err[1], t.err[2]));
			}
		}
	}

	void ProceduralMesh::CompactMesh()
	{
		int dst = 0;
		loopi(0, vertices.Size())
		{
			vertices[i].tcount = 0;
		}

		loopi(0, triangles.Size())
			if (!triangles[i].deleted)
			{
				Triangle &t = triangles[i];
				triangles[dst++] = t;
				loopj(0, 3) vertices[t.v[j]].tcount = 1;
			}

		triangles.Resize(dst);
		dst = 0;
		loopi(0, vertices.Size())
			if (vertices[i].tcount)
			{
				vertices[i].tstart = dst;
				vertices[dst].p = vertices[i].p;
				dst++;
			}

		loopi(0, triangles.Size())
		{
			Triangle &t = triangles[i];
			loopj(0, 3)t.v[j] = vertices[t.v[j]].tstart;
		}

		vertices.Resize(dst);
	}

	double ProceduralMesh::VertexError(SymetricMatrix q, double x, double y, double z)
	{
		return
			q[0] * x*x + 2 * q[1] * x*y + 2 * q[2] * x*z + 2 * q[3] * x + q[4] * y*y
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
			// det = 0 -> try to Find best result
			Vector3 p1 = vertices[id_v1].p;
			Vector3 p2 = vertices[id_v2].p;
			Vector3 p3 = (p1 + p2) / 2;

			double error1 = VertexError(q, p1.x_, p1.y_, p1.z_);
			double error2 = VertexError(q, p2.x_, p2.y_, p2.z_);
			double error3 = VertexError(q, p3.x_, p3.y_, p3.z_);
			error = fmin(error1, fmin(error2, error3));

			// error = fmin(error1, error2);
			if (error1 == error) p_result = p1;
			if (error2 == error) p_result = p2;
			if (error3 == error) p_result = p3;
		}

		return error;
	}
}
