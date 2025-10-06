#!/bin/bash

SHADER_DIR=${1:-"shaders"}
OUTPUT_DIR=${2:-"build/shaders_compiled"}

mkdir -p "$OUTPUT_DIR"

get_shader_stage() {
    case "$1" in
        *.vert.glsl) echo "vert" ;;
        *.frag.glsl) echo "frag" ;;
        *.geom.glsl) echo "geom" ;;
        *.comp.glsl) echo "comp" ;;
        *.tesc.glsl) echo "tesc" ;;
        *.tese.glsl) echo "tese" ;;
    esac
}

find "$SHADER_DIR" -name "*.glsl" | while read shader; do
    stage=$(get_shader_stage "$(basename "$shader")")
    name=$(basename "$shader" .glsl)
    [ -n "$stage" ] && glslc -fshader-stage="$stage" "$shader" -o "$OUTPUT_DIR/$name.spv"
done
