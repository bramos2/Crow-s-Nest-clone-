#pragma once

#include "math_types.hpp"

namespace crow {
	struct super_matrix {
		// scale of matrix
		float3e scale;
		// rotation in degrees
		float3e rotation;
		// synonymous with row 4 of the view matrix
		float3e position;

		// holds final matrices that are sent to the renderer
		DirectX::XMMATRIX final_matrix;
		float4x4 temp;

		super_matrix();
		
		// updates matrices based on the vectors
		void update();

		// updates position vector based on the input matrix
		void update_position(DirectX::XMMATRIX& in);

		void rotate_y_axis_from_velocity(DirectX::XMFLOAT3 in);
	};
}