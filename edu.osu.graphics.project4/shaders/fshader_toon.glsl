#version 150

in vec3 vInterpNormal;
in vec3 vInterpLight;
in vec3 pos;
out vec4 fColor;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;
uniform int flag;
uniform int manipulatorFlag;
uniform int selectionColorR;
uniform int selectionColorG;
uniform int selectionColorB;
uniform int selectionColorA;
uniform int numToonShades;

void main()
{
    vec3 N = vInterpNormal;
    vec3 L = vInterpLight;
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;

    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    }

	if (manipulatorFlag == 1) // hard code x-axis color
	{
		fColor.r = float(255);
		fColor.g = float(0);
		fColor.b = float(0);
		fColor.a = float(255);
	}
	else if (manipulatorFlag == 2) // hard code y-axis color
	{
		fColor.r = float(0);
		fColor.g = float(255);
		fColor.b = float(0);
		fColor.a = float(255);
	}
	else if (manipulatorFlag == 3) // hard code z-axis color
	{
		fColor.r = float(0);
		fColor.g = float(0);
		fColor.b = float(255);
		fColor.a = float(255);
	}
	else
	{
	   if(flag != 1){
	   if (dot(E, N) < 0.35)
	   {
			fColor.r = float(0);
			fColor.g = float(0);
			fColor.b = float(0);
			fColor.a = 1.0;
	   }
	   else
	   {
			//fColor = ambient + diffuse + specular;
			fColor = ambient + diffuse;

			float intensity = dot(L, N);

			for (int i = numToonShades-1; i >= 0; i--)
			{
				if (intensity > 1.0*i/numToonShades)
				{
					fColor = (1.0*i/numToonShades)*fColor;
					break;
				}
			}

			//if (intensity > 0.95)
			//	fColor = fColor;
			//else if (intensity > 0.5)
			//	fColor = .7 * fColor;
			//else if (intensity > 0.05)
			//	fColor = .35 * fColor;
			//else
			//	fColor = .1 * fColor;
			//fColor.a = 1.0;
		}
		} else {
	       		fColor.r = float(selectionColorR)/float(255);
	       		fColor.g = float(selectionColorG)/float(255);
			fColor.b = float(selectionColorB)/float(255);
			fColor.a = float(selectionColorA)/float(255);


		}
	}
}
