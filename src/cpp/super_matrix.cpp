#include "../hpp/super_matrix.hpp"

namespace crow {
    super_matrix::super_matrix() {
        scale.x = scale.y = scale.z = 1.f;
        rotation.x = rotation.y = rotation.z = 0.f;
        position.x = position.y = position.z = 0.f;
    }

    void super_matrix::update() {
        temp = IdentityM();

        float4x4 rot_x = IdentityM();
        float4x4 rot_y = IdentityM();
        float4x4 rot_z = IdentityM();
        
        // firstly, scale
        temp[0][0] *= scale.x;
        temp[1][1] *= scale.y;
        temp[2][2] *= scale.z;

        // then rotate
        rot_x = XrotationM(rot_x, rotation.x);
        rot_y = YrotationM(rot_y, rotation.y);
        rot_z = ZrotationM(rot_z, rotation.z);
        
        temp = MultiplyM(temp, rot_x);
        temp = MultiplyM(temp, rot_y);
        temp = MultiplyM(temp, rot_z);

        // wait, position too
        temp[3][0] = position.x;
        temp[3][1] = position.y;
        temp[3][2] = position.z;

        float4x4_a pre = pac(temp);


        // finalize matrices, ready for drawing
		final_matrix = (DirectX::XMMATRIX&)pre;
    }

    void super_matrix::update_position(DirectX::XMMATRIX& in) {
        float4x4_a _in = (float4x4_a&)in;
        position.x = _in[3][0];
        position.y = _in[3][1];
        position.z = _in[3][2];

    }

    void super_matrix::rotate_y_axis_from_velocity(DirectX::XMFLOAT3 in) {
        if (in.x == 0 && in.z == 0) return;
        float angle_rad = -atan2f(in.x, in.z);
        rotation.y = to_degreesf * angle_rad;
    }

}