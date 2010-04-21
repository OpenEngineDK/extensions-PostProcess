uniform sampler2D fb0color0;
uniform sampler2D fb1color0;
uniform sampler2D fb2color0;
uniform sampler2D fb3color0;
uniform sampler2D fb4color0;
uniform sampler2D fb5color0;
uniform sampler2D fb6color0;
uniform sampler2D depth;

varying vec2 texCoord;

void main () {
    vec4 color = texture2D(fb0color0, texCoord) +
        texture2D(fb1color0, texCoord) +
        texture2D(fb2color0, texCoord) +
        texture2D(fb3color0, texCoord) +
        texture2D(fb4color0, texCoord) +
        texture2D(fb5color0, texCoord) +
        texture2D(fb6color0, texCoord);

    gl_FragColor = color / 7.0 ;
    gl_FragDepth = texture2D(depth, texCoord).x;
}
