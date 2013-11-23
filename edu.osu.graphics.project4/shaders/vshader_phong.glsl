#version 150
in  vec4 vPosition;
in  vec3 vNormal;
out vec3 vInterpNormal;
out vec3 vInterpLight;
out vec3 pos;
uniform vec4 LightPosition;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
	pos = (View * Model * vPosition).xyz;

	gl_Position = Projection * View * Model * vPosition;

	vInterpNormal = normalize( View*Model*vec4(vNormal, 0.0) ).xyz;
	vInterpLight = normalize( (View * LightPosition).xyz - pos );
}
