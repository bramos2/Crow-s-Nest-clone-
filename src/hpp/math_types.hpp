#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include <DirectXMath.h>
//#include "frustum_culling.h"

namespace crow
{
	constexpr double to_radians = 0.01745329251994329576923690768489;
	constexpr float to_radiansf = 0.0174532924f;
	constexpr double to_degrees = 57.295779513082320876798154814105;
	constexpr float to_degreesf = 57.2957802f;

	struct float2e
	{
		float x;
		float y;

		inline float& operator[](int i) { return (&x)[i]; }
		inline float operator[](int i)const { return (&x)[i]; }

		inline float* data() { return &x; }
		inline const float* data()const { return &x; }
		inline static constexpr size_t size(){ return 2; }
		
		float2e() : x(0), y(0) {}
		float2e(float _x, float _y) : x(_x), y(_y) { }
		float2e& operator=(const float2e& that)
		{
			if (this != &that)
			{
				x = that.x;
				y = that.y;
			}
			return *this;
		}
		inline friend bool operator==(const float2e& lhs, const float2e& rhs) {
			if (lhs.x == rhs.x && lhs.y == rhs.y) return true;
			return false;
		}

		inline friend float2e operator+(float2e lhs, float2e rhs)
		{
			return { lhs.x + rhs.x, lhs.y + rhs.y };
		}

		inline friend float2e operator-(float2e lhs, float2e rhs)
		{
			return { lhs.x - rhs.x, lhs.y - rhs.y };
		}

		inline const float length() const { return sqrtf(x*x + y*y);  }
		

		inline float2e normalize()
		{
			float length = this->length();
			if (length == 0.0f)
			{
				float2e value = { 0.0f, 0.0f };
				return value;
			}
			float2e value = { x / length, y / length };
			return value;
		}

	};

	struct float3e
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
			};

			float2e xy;
		};

		float3e() : x(0), y(0), z(0) {}
		float3e(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
		float3e(const DirectX::XMFLOAT3 that) : x(that.x), y(that.y), z(that.z) { }

		float3e& operator=(const float3e& that)
		{
			if (this != &that)
			{
				x = that.x;
				y = that.y;
				z = that.z;
			}
			return *this;
		}

		inline float& operator[](int i) { return (&x)[i]; }
		inline float operator[](int i)const { return (&x)[i]; }

		inline float* data() { return &x; }
		inline const float* data()const { return &x; }
		inline static constexpr size_t size(){ return 3; }

		inline friend float3e operator+(float3e lhs, float3e rhs)
		{
			return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
		}

		inline friend float3e operator-(float3e lhs, float3e rhs)
		{
			return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
		}

		inline friend float3e operator*(float3e lhs, float3e rhs)
		{
			return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
		}

		inline friend float3e operator*(float3e lhs, float rhs)
		{
			return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
		}

		inline friend float3e operator/(float3e lhs, float3e rhs)
		{
			return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
		}

		inline friend float3e operator/(float3e lhs, float rhs)
		{
			return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
		}

		inline float dot(float3e lhs, float3e rhs)
		{
			return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
		}

		inline float3e cross(float3e lhs, float3e rhs)
		{
			return { lhs.y*rhs.z - lhs.z*rhs.y, lhs.z*rhs.x - lhs.x*rhs.z, lhs.x*rhs.y - lhs.y*rhs.x };
		}

		inline float3e normalize(float3e n)
		{
			float length = sqrtf(n.x*n.x + n.y*n.y + n.z*n.z);
			if (length == 0.0f)
			{
				float3e value = { 0.0f, 0.0f, 0.0f };
				return value;
			}
			float3e value = { n.x / length, n.y / length, n.z / length };
			return value;
		}

		inline float3e& operator+=(float3e rhs)
		{
			x += rhs.x;
			y += rhs.y; 
			z += rhs.z;

			return *this;
		}

		inline float3e& operator-=(float3e rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;

			return *this;
		}

		inline float3e& operator*=(float3e rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;

			return *this;
		}

		inline float3e& operator/=(float3e rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;

			return *this;
		}

		inline float3e& operator*=(float rhs)
		{
			x *= rhs;
			y *= rhs;
			z *= rhs;

			return *this;
		}

		inline float3e& operator/=(float rhs)
		{
			x /= rhs;
			y /= rhs;
			z /= rhs;

			return *this;
		}
	};

	struct float4e
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};

			float3e xyz;
			
			struct
			{
				float2e xy;
				float2e zw;
			};
		};

		float4e() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		float4e(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }

		inline float& operator[](int i) { return (&x)[i]; }
		inline float operator[](int i)const { return (&x)[i]; }

		inline float* data() { return &x; }
		inline const float* data()const { return &x; }
		inline static constexpr size_t size() { return 4; }

		float4e& operator=(const float4e& that)
		{
			if (this != &that)
			{
				x = that.x;
				y = that.y;
				z = that.z;
				w = that.w;
			}
			return *this;
		}

		inline float4e& operator+=(float4e rhs)
		{
			x += rhs.x;
			y += rhs.y; 
			z += rhs.z;
			w += rhs.w;

			return *this;
		}

		inline friend float4e operator*(float4e lhs, float rhs)
		{
			return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs };
		}

		inline friend float4e operator/(float4e lhs, float rhs)
		{
			return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs };
		}

		inline float4e normalize(float4e n)
		{
			float length = sqrtf(n.x*n.x + n.y*n.y + n.z*n.z);
			if (length == 0.0f)
			{
				float4e value = { 0.0f, 0.0f, 0.0f, 0.0f };
				return value;
			}
			float4e value = { n.x / length, n.y / length, n.z / length, n.w / length };
			return value;
		}

		inline float4e& operator*=(float rhs)
		{
			x *= rhs;
			y *= rhs;
			z *= rhs;
			w *= rhs;

			return *this;
		}

		inline float4e& operator/=(float rhs)
		{
			x /= rhs;
			y /= rhs;
			z /= rhs;
			w /= rhs;

			return *this;
		}
	};

	struct alignas(8) float2_a : float2e {};

	struct alignas(16) float3_a : float3e {};

	struct alignas(16) float4_a : float4e {};

	using float4x4 = std::array< float4e, 4 >;
	using float4x4_a = std::array< float4_a, 4 >;
	
	inline float4x4 IdentityM()
	{
		float4x4 temp;
		temp[0] = { 1.0f,0,0,0 };
		temp[1] = { 0,1.0f,0,0 };
		temp[2] = { 0,0,1.0f,0 };
		temp[3] = { 0,0,0,1.0f };

		return temp;
	}

	inline float4x4_a IdentityM_a()
	{
		float4x4_a temp;
		temp[0][0] = 1.0f; temp[0][1] = 0.0f; temp[0][2] = 0.0f; temp[0][3] = 0.0f; 
		temp[1][0] = 0.0f; temp[1][1] = 1.0f; temp[1][2] = 0.0f; temp[1][3] = 0.0f; 
		temp[2][0] = 0.0f; temp[2][1] = 0.0f; temp[2][2] = 1.0f; temp[2][3] = 0.0f; 
		temp[3][0] = 0.0f; temp[3][1] = 0.0f; temp[3][2] = 0.0f; temp[3][3] = 1.0f; 

		return temp;
	}

	inline float4e MatrixVectorMult(float4e v, float4x4 m)
	{
		float4e result = { 0,0,0,0 };

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				result[i] += v[j] * m[j][i];
			}
		}
		return result;
	}
	
	inline float4x4 MultiplyM(float4x4 a, float4x4 b)
	{
		float4x4 result;
		for (int i = 0; i < 4; ++i)
		{
			result[i] = MatrixVectorMult(a[i], b);
		}
		return result;
	}

	inline float4_a MatrixVectorMult(float4_a v, float4x4_a m)
	{
		float4_a result;
		result[0] = result[1] = result[2] = result[3] = 0;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				result[i] += v[j] * m[j][i];
			}
		}
		return result;
	}

	inline float4x4_a MultiplyM(float4x4_a a, float4x4_a b)
	{
		float4x4_a result;
		for (int i = 0; i < 4; ++i)
		{
			result[i] = MatrixVectorMult(a[i], b);
		}
		return result;
	}

	inline float4x4 XrotationM(float4x4 m, float deg)
	{
		deg = deg * (3.1415f / 180.0f);

		float4x4 temp = IdentityM();
		temp[1].y = cosf(deg);
		temp[1].z = -sinf(deg);
		temp[2].y = sinf(deg);
		temp[2].z = cosf(deg);
		return temp;
	}

	inline float4x4 YrotationM(float4x4 m, float deg)
	{
		deg = deg * (3.1415f / 180.0f);

		float4x4 temp = IdentityM();
		temp[0].x = cosf(deg);
		temp[0].z = sinf(deg);
		temp[2].x = -sinf(deg);
		temp[2].z = cosf(deg);
		return temp;
	}

	inline float4x4 ZrotationM(float4x4 m, float deg)
	{
		deg = deg * (3.1415f / 180.0f);

		float4x4 temp = IdentityM();
		temp[0].x = cosf(deg);
		temp[0].y = -sinf(deg);
		temp[1].x = sinf(deg);
		temp[1].y = cosf(deg);
		return temp;

	}

	inline float4x4_a XrotationM(float4x4_a m, float deg)
	{
		deg = deg * (3.1415f / 180.0f);

		float4x4_a temp = IdentityM_a();
		temp[1].y = cosf(deg);
		temp[1].z = -sinf(deg);
		temp[2].y = sinf(deg);
		temp[2].z = cosf(deg);
		return MultiplyM(temp, m);
	}

	inline float4x4_a YrotationM(float4x4_a m, float deg)
	{
		deg = deg * (3.1415f / 180.0f);

		float4x4_a temp = IdentityM_a();
		temp[0].x = cosf(deg);
		temp[0].z = sinf(deg);
		temp[2].x = -sinf(deg);
		temp[2].z = cosf(deg);
		return MultiplyM(temp, m);
	}

	inline float4x4_a ZrotationM(float4x4_a m, float deg)
	{
		deg = deg * (3.1415f / 180.0f);

		float4x4_a temp = IdentityM_a();
		temp[0].x = cosf(deg);
		temp[0].y = -sinf(deg);
		temp[1].x = sinf(deg);
		temp[1].y = cosf(deg);
		return MultiplyM(temp, m);

	}
	
	inline void scale_matrix(float4x4_a& m, float x, float y, float z)
	{
		m[0][0] *= x;
		m[1][1] *= y;
		m[2][2] *= z;
	}

	inline float3_a pac(float3e in) {
		float3_a temp;
		temp.x = in.x;
		temp.y = in.y;
		temp.z = in.z;
		return temp;
	}

	inline float4x4_a pac(float4x4 in) {
		float4x4_a temp;
		temp[0][0] = in[0][0]; temp[0][1] = in[0][1]; temp[0][2] = in[0][2]; temp[0][3] = in[0][3]; 
		temp[1][0] = in[1][0]; temp[1][1] = in[1][1]; temp[1][2] = in[1][2]; temp[1][3] = in[1][3]; 
		temp[2][0] = in[2][0]; temp[2][1] = in[2][1]; temp[2][2] = in[2][2]; temp[2][3] = in[2][3]; 
		temp[3][0] = in[3][0]; temp[3][1] = in[3][1]; temp[3][2] = in[3][2]; temp[3][3] = in[3][3]; 

		return temp;
	}
	
	inline float clampf(float in, float min, float max) {
		return in < min ? min : (in > max ? max : in);
	}
}

namespace crow
{
	struct colored_vertex
	{
		float3e pos = { 0.0f, 0.0f, 0.0f };
		float4e color = { 1.0f, 1.0f, 1.0f, 1.0f };

		colored_vertex() = default;
		colored_vertex(const colored_vertex&) = default;

		inline colored_vertex(const float3e& p, const float4e& c) : pos{ p }, color{ c } {}
		inline colored_vertex(const float3e& p, const float3e& c) : pos{ p }, color{ c.x, c.y, c.z, 1.0f } {}
		inline colored_vertex(const float3e& p, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : pos{ p }, color{ r/255.0f, g/255.0f, b/255.0f, a/255.0f } {}
	};

	struct textured_vertex {
		float3e pos = { 0.0f, 0.0f, 0.0f };
		float size = 0;
		float transparency = 0;
		float rotation = 0;

		// cast this to a texture pointer later
		void* texture;
	};

	struct mesh_vertex
	{
		float pos[3] = { 0.0f, 0.0f, 0.0f };
		float normal[3] = { 0.0f, 0.0f, 0.0f };
		float uv[2] = { 0.0f, 0.0f };
		int joints[4] = { 0,0,0,0 };  //joint indicies
		float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f}; //joint weights

		mesh_vertex() = default;
		mesh_vertex(const mesh_vertex&) = default;

		//inline mesh_vertex(const float3e& p, const float3e& n, const float2e& u) : pos{ p }, normal{ n }, uv{ u } {}
	};

	static float randFloat(float min, float max)
	{
		return (max - min) * (rand() / (float)RAND_MAX) + min;
	}

	static int randInt(int min, int max)
	{
		return rand() % (max - min) + min;
	}

	struct mat_t
	{
		enum eComp { EMISSIVE = 0, DIFFUSE, SPECULAR, SHININESS, COUNT };

		struct comp_t
		{
			float value[3] = { 0.0f,0.0f,0.0f };
			float factor = 0.0f;
			int64_t input = -1;
		};

		comp_t& operator[](int i) { return components[i]; }
		const comp_t& operator[](int i)const { return components[i]; }

	private:
		comp_t components[COUNT];

	};
}