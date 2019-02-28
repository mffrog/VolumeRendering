#version 450

layout(std140, binding = 1) buffer CameraData{
	vec4 resolution;
	vec4 position;
	vec4 forward;
	vec4 up;
} camera;

struct Cell{
	vec4 data;
};

layout(std140, binding = 2) buffer VolumeData{
	mat4 worldInverse;
	vec4 cellSize;
	Cell cells[];
};

Cell GetCell(vec3 coord){
	if(0 > coord.x || 1.0 < coord.x || 0 > coord.y || 1.0 < coord.y || 0 > coord.z || 1.0 < coord.z){
		Cell dummy;
		dummy.data = vec4(0.0);
		return dummy;
	}
	return cells[int(coord.x * cellSize.x) + int(coord.y * cellSize.y) * int(cellSize.x) + int(coord.z * cellSize.z) * int(cellSize.x * cellSize.y)];
}

Cell GetEncloseCell(vec3 worldPos){
	vec3 localPos = (worldInverse * vec4(worldPos,1.0)).xyz;
	localPos += vec3(1.0);
	localPos *= 0.5;
	return GetCell(localPos);
}

struct Ray{
	vec3 position;
	vec3 direction;
};

out vec4 fragColor;

void main(){
	vec2 coord = (gl_FragCoord.xy * 2.0 - camera.resolution.xy) / ( min(camera.resolution.x, camera.resolution.y)) ;

	vec3 right = cross(camera.forward.xyz,camera.up.xyz);
	float depthTarget = camera.resolution.z;
	Ray ray;
	ray.position = camera.position.xyz;
	ray.direction = normalize(camera.forward.xyz * depthTarget + camera.up.xyz * coord.y + right * coord.x);

	int itelation = 512;
	float length = 30.0;
	float perLen = length / itelation;
	float opacity = 0.0f;
	for(int i = 0; i < itelation; ++i){
		vec3 worldPos = ray.position + (i * perLen) * ray.direction;
		vec4 res = GetEncloseCell(worldPos).data;
		
		float a = res.a;
		float o = (1 - opacity);
		opacity += a * o;
		res.rgb *= a;
		fragColor.rgb += o * res.rgb; 
		/*
		fragColor += res;
		*/
		if(opacity > 1.0){
			break;
		}
	}
	fragColor.a = opacity;
}