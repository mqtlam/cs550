#version 150
in  vec4 vPosition;
in  vec3 vNormal;
out vec4 color;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
//uniform mat4 ModelView;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform float Shininess;
uniform int flag;
uniform int manipulatorFlag;
uniform int selectionColorR;
uniform int selectionColorG;
uniform int selectionColorB;
uniform int selectionColorA;

void main()
{
    // Transform vertex  position into eye coordinates
    //vec3 pos = (ModelView * vPosition).xyz;
	vec3 pos = (View * Model * vPosition).xyz;

    vec3 L = normalize( (View * LightPosition).xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
    //vec3 N = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;
	vec3 N = normalize( View*Model*vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;

    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    }

    //gl_Position = Projection * ModelView * vPosition;
	gl_Position = Projection * View * Model * vPosition;

	if (manipulatorFlag == 1) // hard code x-axis color
	{
		color.r = float(255);
		color.g = float(0);
		color.b = float(0);
		color.a = float(255);
	}
	else if (manipulatorFlag == 2) // hard code y-axis color
	{
		color.r = float(0);
		color.g = float(255);
		color.b = float(0);
		color.a = float(255);
	}
	else if (manipulatorFlag == 3) // hard code z-axis color
	{
		color.r = float(0);
		color.g = float(0);
		color.b = float(255);
		color.a = float(255);
	}
	else
	{
	   if(flag != 1){
		color = ambient + diffuse + specular;
		color.a = 1.0;
		} else {
	       		color.r = float(selectionColorR)/float(255);
	       		color.g = float(selectionColorG)/float(255);
			color.b = float(selectionColorB)/float(255);
			color.a = float(selectionColorA)/float(255);


		}
	}
}
