#version 430
precision highp float;

// OUTPUT
out vec4 oFragmentColor;

// INPUT
in vec2 texCoord;

// Simple 2D noise function based on texCoord
float noise(vec2 uv) {
    return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float sdCircle( vec2 p, float r )
{
    return length(p) - r;
}


float sdMoon(vec2 p, float d, float ra, float rb )
{
    p.y = abs(p.y);
    float a = (ra*ra - rb*rb + d*d)/(2.0*d);
    float b = sqrt(max(ra*ra-a*a,0.0));
    if( d*(p.x*b-p.y*a) > d*d*max(b-p.y,0.0) )
          return length(p-vec2(a,b));
    return max( (length(p          )-ra),
               -(length(p-vec2(d,0))-rb));
}

// MAIN PROGRAM
void main()
{
    // Les coordonnées UV sont entre 0 et 1
	vec2 uv = mod(5.0 * texCoord, 1.0);

	// Génération de bruit avec une autre fonction
    float noiseValue = noise(uv);

    // Seuil de bruit pour utiliser discard
    float threshold = 0.3;

    // Si la valeur du bruit est inférieure au seuil, on discard le fragment
    if (noiseValue < threshold) {
        discard;
    }

    // Couleurs basées sur les coordonnées UV et les variations
    
	//blur cercle blanc sur fond noir
	//vec3 color = vec3(1.0,1.0,1.0)* (-(sdCircle(uv-vec2(0.5),0.5)));
	float D= sdMoon(uv-vec2(0.5),0.25,0.35,0.12)*(sdMoon(uv-vec2(0.5),0.25,0.27,0.20));
	vec3 color;
    if (D < 0.0) {
        color = vec3(1.0); // Couleur blanche à l'intérieur
    } else {
        color = vec3(uv,0.); // Couleur noire à l'extérieur
    }

    // Output final avec alpha
    oFragmentColor = vec4(color, 1.0);
}
