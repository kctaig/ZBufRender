#pragma once

template <typename vertex, typename uniform>
class Shader
{
public:

    using vertexShader = void (*)(const vertex&, const uniform&);
    using fragmentShader = void (*)(const uniform&);


    Shader(const vertexShader &vs, const fragmentShader &fs) : vs(vs), fs(fs) {}

private:
    vertexShader vs;
    fragmentShader fs;
};