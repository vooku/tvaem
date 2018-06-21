static const auto computeShader = 
    "#version 440\n"
    
    "layout(rgba8, binding = 0) uniform readonly image2D layer0;\n"
    "layout(rgba8, binding = 1) uniform readonly image2D layer1;\n"
    "layout(rgba8, binding = 2) uniform readonly image2D layer2;\n"
    "layout(rgba8, binding = 3) uniform readonly image2D layer3;\n"
    "layout(rgba8, binding = 4) uniform readonly image2D layer4;\n"
    "layout(rgba8, binding = 5) uniform readonly image2D layer5;\n"
    "layout(rgba8, binding = 6) uniform readonly image2D layer6;\n"
    "layout(rgba8, binding = 7) uniform writeonly image2D dst;\n"
    "layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;\n"
    
    "uniform int nLayers;\n"
    "uniform bool[7] playing;\n"
    "uniform ivec2[7] dimensions;\n"
    "uniform int[7] blendingModes;\n"
    "uniform bool inverse;\n"
    "uniform bool reducePalette;\n"
    "uniform bool colorShift;\n"
    "uniform bool colorShift2;\n"
    
    "void main(){\n"
    "	const ivec2 xy = ivec2(gl_GlobalInvocationID.xy);\n"
    "	const vec2 globalDims = gl_WorkGroupSize.xy * gl_NumWorkGroups.xy;\n"
    "	vec3 colors[7];\n"
    "	colors[0] = imageLoad(layer0, ivec2(xy / globalDims * dimensions[0])).rgb;\n"
    "	colors[1] = imageLoad(layer1, ivec2(xy / globalDims * dimensions[1])).rgb;\n"
    "	colors[2] = imageLoad(layer2, ivec2(xy / globalDims * dimensions[2])).rgb;\n"
    "	colors[3] = imageLoad(layer3, ivec2(xy / globalDims * dimensions[3])).rgb;\n"
    "	colors[4] = imageLoad(layer4, ivec2(xy / globalDims * dimensions[4])).rgb;\n"
    "	colors[5] = imageLoad(layer5, ivec2(xy / globalDims * dimensions[5])).rgb;\n"
    "	colors[6] = imageLoad(layer6, ivec2(xy / globalDims * dimensions[6])).rgb;\n"
    "	vec3 blended = vec3(0.0);\n"
    "	bool anyPlaying = false;\n"
    
    "	for (int i = 0; i < nLayers; i++) {\n"
    "		if (playing[i]) {\n"
    "			anyPlaying = true;\n"
    "			switch(blendingModes[i]) {\n"
    "				case 1: // Multiply\n"
    "					blended = blended * colors[i];\n"
    "					break;\n"
    "				case 2: // Screen\n"
    "					blended = 1.0 - (1.0 - blended) * (1.0 - colors[i]);\n"
    "					break;\n"
    "				case 3: // Darken\n"
    "					blended = min(blended, colors[i]);\n"
    "					break;\n"
    "				case 4: // Lighten\n"
    "					blended = max(blended, colors[i]);\n"
    "					break;\n"
    "				case 5: // LinearDodge\n"
    "					blended = blended + colors[i];\n"
    "					break;\n"
    "				case 6: // Difference\n"
    "					blended = abs(blended - colors[i]);\n"
    "					break;\n"
    "				case 7: // Exclusion\n"
    "					blended = blended + colors[i] + 2.0 * blended * colors[i];\n"
    "					break;\n"
    "				default:\n"
    "					blended = colors[i];\n"
    "					break;\n"
    "			}\n"
    "			blended = clamp(vec3(0.0), vec3(1.0), blended);\n"
    "		}\n"
    "	}\n"
    "	if (anyPlaying) {\n"
    "		blended = mix(blended, vec3(1.0) - blended, inverse);\n"
    "		blended = mix(blended, round(blended), reducePalette);\n"
    "		blended = mix(blended, blended.brg, colorShift);\n"
    "		blended = mix(blended, blended.gbr, colorShift2);\n"
    "	}\n"
    
    "	imageStore(dst, xy, vec4(blended, 1.0));\n"
    "}\n"
;