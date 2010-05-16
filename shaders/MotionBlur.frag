uniform sampler2D fb0color0;
uniform sampler2D fb1Color0;

uniform float bias;

varying vec2 texCoord;

void main() {

    vec4 color = texture2D(fb0color0, texCoord);
    vec4 prevColor = texture2D(fb1color0, texCoord);

    gl_FragColor = mix(color, prevColor, bias);
}
