#include "../hpp/view.hpp"

namespace crow {
    

    void view_t::init() {
        position.x = 0;
        position.y = 0;
        position.z = 0;

        rotation.x = 0;
        rotation.y = 0;
        rotation.z = 0;
        
        // evaluate projection matrix, only need to do this once
        proj_final = DirectX::XMMatrixTranspose((DirectX::XMMATRIX&)proj_mat);

        // rotation matrix init
        rot_mat = IdentityM();
    }

    void view_t::update() {
        view_mat = pac(rot_mat);

        // update position
        view_mat[3][0] = position.x;
        view_mat[3][1] = position.y;
        view_mat[3][2] = position.z;

        // finalize matrices, ready for drawing
		view_final = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, (DirectX::XMMATRIX&)view_mat));
        // projection matrix does not change, and will not be reevaluated when update() is called
		//proj_final = DirectX::XMMatrixTranspose((DirectX::XMMATRIX&)proj_mat);
    }

    void view_t::update_rotation_matrix() {
        //float4x4 rot_x = IdentityM();
        float4x4 rot_y = IdentityM();
        float4x4 rot_z = IdentityM();
        rot_mat = IdentityM();

        // rotate camera
        //rot_x = XrotationM(rot_x, rotation.x);
        rot_y = YrotationM(rot_y, rotation.y);
        rot_z = ZrotationM(rot_z, rotation.z);
        rot_mat = XrotationM(rot_mat, rotation.x);
        
        //rot_mat = MultiplyM(rot_mat, rot_x);
        rot_mat = MultiplyM(rot_mat, rot_y);
        rot_mat = MultiplyM(rot_mat, rot_z);
    }

	float3e mouse_to_floor(view_t camera, float2e mouse_pos, int w, int h) {
        float3e ray_origin = camera.position;

        float3e ray_direction = mouse_to_ray(camera, mouse_pos, w, h);
        float3e final_position = ray_to_floor(ray_direction, ray_origin);

        return final_position;  //{clicked_position.x, invalid, clicked_position.z};
    }

	float3e mouse_to_ray(view_t camera, float2e mouse_pos, int w, int h) {

        // first thing we gotta get is the position of the pixel we clicked with
        // the mouse and move it into ndc
        DirectX::XMVECTOR mouse_position_ndc = {(mouse_pos.x / w * 2 - 1),
                                                (mouse_pos.y / h * 2 - 1), 1, 1};
        
        // then we multiply this point by the inverse of the projection matrix to
        // move it into camera space
        DirectX::XMMATRIX inverse_projection_matrix = DirectX::XMMatrixInverse(nullptr, camera.proj_final);
        DirectX::XMVECTOR mouse_position_cam = DirectX::XMVector4Transform(mouse_position_ndc, inverse_projection_matrix);

        //float4x4_a ipm = (float4x4_a&)inverse_projection_matrix;
        //float4_a ndc = (float4_a&)mouse_position_ndc;
        //float4x4 _ipm = {ipm[0], ipm[1], ipm[2], ipm[3] };
        //float4e _ndc = { ndc.x, ndc.y, ndc.z, ndc.w };
        //float4e _mpc = MatrixVectorMult(_ndc, _ipm);
        
        // blank out these last two values so they dont screw us up
		DirectX::XMFLOAT4 f4_temp;
		DirectX::XMStoreFloat4(&f4_temp, mouse_position_ndc);
		DirectX::XMStoreFloat4(&f4_temp, mouse_position_cam);
        f4_temp.z = 1;
        f4_temp.w = 0;
		mouse_position_cam = DirectX::XMLoadFloat4(&f4_temp);

        // continue backpedaling by multiplying by the inverse view matrix
        // to move it into world space
        DirectX::XMMATRIX inverse_view_matrix = DirectX::XMMatrixTranspose((DirectX::XMMATRIX&)camera.view_mat); // already inverted, but needs to be transposed
        DirectX::XMVECTOR mouse_position_wld = DirectX::XMVector4Transform(mouse_position_cam, inverse_view_matrix);
        // the variables of the ray itself; its origin and its direction
		DirectX::XMStoreFloat4(&f4_temp, mouse_position_wld);
        float4e ray_direction = float4e(f4_temp.x, f4_temp.y, f4_temp.z, f4_temp.w);
        ray_direction = ray_direction.normalize(ray_direction);

        return { ray_direction.x, -ray_direction.y, ray_direction.z };
    }

    float3e ray_to_floor(float3e ray_direction, float3e ray_position) {
        float3e final_position = ray_position;
        // "invalid" float: if the clicked position does not return a valid position,
        // this float will be set to -1. in any method taking in the resultant of this
        // method, check for this -1. that means the position returned by this method
        // cannot be used.
        float invalid = 0;
        // ray is pointing at the floor
        if (ray_direction.y < 0 && ray_position.y > 0 || ray_direction.y > 0 && ray_position.y < 0) {
            final_position.x -= ray_position.y * (ray_direction.x / ray_direction.y);
            // although unnecessaary, the y position is "calculated"
            final_position.y -= ray_position.y;  // * (ray_direction.y / ray_direction.y);
            final_position.z -= ray_position.y * (ray_direction.z / ray_direction.y);
            // ray is not pointing at the floor and thus will never intersect
        } else {
            invalid = -1;
        }

        final_position.y = invalid;
        return final_position;
    }

    void update_room_cam(crow::room* active_room, view_t& camera) {
       // update_room_cam(pac(active_room->cam_pos), pac(active_room->cam_rotation), camera);
    }

	void update_room_cam(float3_a position, float3_a rotation, view_t& camera) {
        camera.position = position;
        camera.rotation = rotation;
        camera.update_rotation_matrix();
        camera.update();
    }

}  // namespace crow
