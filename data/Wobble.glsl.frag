uniform sampler2D color0;
uniform sampler2D depth;

const vec2 offset = vec2(0.0, 0.0);
varying vec2 texCoord;

void main () {
    vec2 texcoord = texCoord;
    
    const vec2 freq     = vec2(10,10);
    const vec2 strength = vec2(0.1, 0.1);
    
    vec2 newTexcoord;
    newTexcoord.x = texcoord.x + sin(texcoord.y * freq.x + offset.x) * strength.x;
    newTexcoord.y = texcoord.y + sin(texcoord.x * freq.y + offset.y) * strength.y;

    gl_FragColor = texture2D(color0, newTexcoord);
    gl_FragDepth = texture2D(depth, newTexcoord).x;
}
