//**************************************************************************
// Source at https://github.com/NVIDIAGameWorks/Falcor/blob/master/Source/RenderPasses/Antialiasing/FXAA/FXAA.slang
// Licensed under the MIT License.
//
/***************************************************************************
 # Copyright (c) 2015-21, NVIDIA CORPORATION. All rights reserved.
 #
 # Redistribution and use in source and binary forms, with or without
 # modification, are permitted provided that the following conditions
 # are met:
 #  * Redistributions of source code must retain the above copyright
 #    notice, this list of conditions and the following disclaimer.
 #  * Redistributions in binary form must reproduce the above copyright
 #    notice, this list of conditions and the following disclaimer in the
 #    documentation and/or other materials provided with the distribution.
 #  * Neither the name of NVIDIA CORPORATION nor the names of its
 #    contributors may be used to endorse or promote products derived
 #    from this software without specific prior written permission.
 #
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND ANY
 # EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 # IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 # PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 # CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 # EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 # PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 # PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 # OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 # (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 # OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **************************************************************************/

#include <Shaders/ConstantBuffers.inc>

#ifndef FXAA_QUALITY__PRESET
#define FXAA_QUALITY__PRESET 39
#endif

//   FXAA Quality - Low Quality, Moderate Dithering
#if (FXAA_QUALITY__PRESET == 10)
#define FXAA_QUALITY__PS 3 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.5, 3.0, 12.0 };
#endif

#if (FXAA_QUALITY__PRESET == 11)
#define FXAA_QUALITY__PS 4 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 3.0, 12.0 };
#endif

#if (FXAA_QUALITY__PRESET == 12)
#define FXAA_QUALITY__PS 5 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 4.0, 12.0 };
#endif

#if (FXAA_QUALITY__PRESET == 13)
#define FXAA_QUALITY__PS 6 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 4.0, 12.0 };
#endif

#if (FXAA_QUALITY__PRESET == 14)
#define FXAA_QUALITY__PS 7 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 4.0, 12.0 };
#endif

#if (FXAA_QUALITY__PRESET == 15)
#define FXAA_QUALITY__PS 8 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 12.0 };
#endif

//   FXAA Quality - Medium, Less Dithering
#if (FXAA_QUALITY__PRESET == 20)
#define FXAA_QUALITY__PS 3 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.5, 2.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 21)
#define FXAA_QUALITY__PS 4 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 22)
#define FXAA_QUALITY__PS 5 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 23)
#define FXAA_QUALITY__PS 6 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 24)
#define FXAA_QUALITY__PS 7 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 3.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 25)
#define FXAA_QUALITY__PS 8 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 26)
#define FXAA_QUALITY__PS 9
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 27)
#define FXAA_QUALITY__PS 10
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 28)
#define FXAA_QUALITY__PS 11
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 29)
#define FXAA_QUALITY__PS 12
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif

//   FXAA Quality - High
#if (FXAA_QUALITY__PRESET == 39)
#define FXAA_QUALITY__PS 12
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif

// cbuffer CB : register(b0)
// {
//     float2 g_TexelSize;
    
//     // Controls sharpness level
//     // 1.00 - Soft
//     // 0.75 - Default filter value
//     // 0.50 - Sharper, removes less subpixel aliasing
//     // 0.25 - Almost off
//     // 0.00 - Completely off
//     float g_QualitySubPix;
    
    // // Threshold control for required local contrast
    // // 0.333 - Very low (faster)
    // // 0.250 - Low quality
    // // 0.166 - Default
    // // 0.125 - High quality
    // // 0.063 - Very high (slower)
    // float g_QualityEdgeThreshold;
    
    // // Threshold for ignoring dark areas
    // // 0.0833 - Default
    // // 0.0625 - Slightly faster
    // // 0.0312 - Slower
    // float g_QualityEdgeThresholdMin;

//     float3 g_padding;
// }

SamplerState g_SamplerLinearClamp : register(s0);
Texture2D<float4> g_TextureInput : register(t0);


//
// Luminance Calculation
//

float LinearRGBToLuminance(float3 LinearRGB)
{
    // return dot(LinearRGB, float3(0.299f, 0.587f, 0.114f));
    return dot(LinearRGB, float3(0.212671f, 0.715160, 0.072169));
    // return sqrt(dot(LinearRGB, float3(0.212671f, 0.715160, 0.072169)));
    // return log2(1 + dot(LinearRGB, float3(0.212671, 0.715160, 0.072169)) * 15) / 4;
}

void main(

	// Input
	//======
	in const float4 i_fragmentPosition : SV_POSITION,
	in const float2 i_fragmentUV : TEXCOORD,

	// Output
	//=======
	out float4 o_color : SV_TARGET

)
{
    float2 posM = i_fragmentUV;
    float4 color = g_TextureInput.SampleLevel(g_SamplerLinearClamp, i_fragmentUV, 0);  

    //   N
    // W M E
    //   S
    float lumaM = LinearRGBToLuminance(color.rgb);
    float lumaS = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posM, 0, int2(0, 1)).rgb);
    float lumaE = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posM, 0, int2(1, 0)).rgb);
    float lumaN = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posM, 0, int2(0, -1)).rgb);
    float lumaW = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posM, 0, int2(-1, 0)).rgb);

    //
    // Calculate contrast to determine if anti-aliasing should be applied
    //
    
    // Find the maxinmum and minimum relative luminance among 5 pixels to get contrast
    float lumaRangeMax = max(lumaM, max(max(lumaW, lumaE), max(lumaN, lumaS)));
    float lumaRangeMin = min(lumaM, min(min(lumaW, lumaE), min(lumaN, lumaS)));
    float lumaRange = lumaRangeMax - lumaRangeMin;
    // If the brightness change is below a threshold that is positively correlated with max brightness,
    // or below an absolute threshold, it indicates a  non-edge region, so no anti-aliasing is applied
    bool earlyExit = lumaRange < max(g_QualityEdgeThresholdMin, lumaRangeMax * g_QualityEdgeThreshold);

    // Early exit if threshold is not met
    if (earlyExit) {
        o_color = color;
        return;
    }

    //
    // Determine if the edge is locally horizontal or vertical
    //

    //           
    //  NW N NE          
    //  W  M  E
    //  WS S SE   
    //  edgeHorz = |(NW - W) - (W - WS)| + 2|(N - M) - (M - S)| + |(NE - E) - (E - SE)|
    //  edgeVert = |(NE - N) - (N - NW)| + 2|(E - M) - (M - W)| + |(SE - S) - (S - WS)|
    float lumaNW = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posM, 0, int2(-1, -1)).rgb);
    float lumaSE = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posM, 0, int2(1, 1)).rgb);
    float lumaNE = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posM, 0, int2(1, -1)).rgb);
    float lumaSW = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posM, 0, int2(-1, 1)).rgb);

    float lumaNS = lumaN + lumaS;
    float lumaWE = lumaW + lumaE;
    float lumaNESE = lumaNE + lumaSE;
    float lumaNWNE = lumaNW + lumaNE;
    float lumaNWSW = lumaNW + lumaSW;
    float lumaSWSE = lumaSW + lumaSE;

    // Calculate horizontal and vertical contrast
    float edgeHorz = abs(lumaNWSW - 2.0 * lumaW) + abs(lumaNS - 2.0 * lumaM) * 2.0 + abs(lumaNESE - 2.0 * lumaE);
    float edgeVert = abs(lumaSWSE - 2.0 * lumaS) + abs(lumaWE - 2.0 * lumaM) * 2.0 + abs(lumaNWNE - 2.0 * lumaN);

    // Determine if it is a local horizontal or vertical edge
    bool horzSpan = edgeHorz >= edgeVert;

    //
    // Calculate gradient, determine edge direction
    //
    float luma1 = horzSpan ? lumaN : lumaW;
    float luma2 = horzSpan ? lumaS : lumaE;

    float gradient1 = luma1 - lumaM;
    float gradient2 = luma2 - lumaM;
    // Compute the gradient in the corresponding directijon and scale it for later comparision
    float gradientScaled = max(abs(gradient1), abs(gradient2)) * 0.25f;
    // Determine which direction is steepset
    bool is1Steepest = abs(gradient1) >= abs(gradient2);

    //
    // Move the current pixel by half a texel along the gradient direction
    //
    float lengthSign = horzSpan ? g_TexelSize_Screen.y : g_TexelSize_Screen.x;
    lengthSign = is1Steepest ? -lengthSign : lengthSign;

    float2 posB = posM.xy;
    // Half texel offset
    if (!horzSpan)
        posB.x += lengthSign * 0.5;
    if (horzSpan)
        posB.y += lengthSign * 0.5;

    //
    // Calculate the average luminance of the two pixels adjacent to posB
    //
    float luma3 = luma1 + lumaM;
    float luma4 = luma2 + lumaM;
    float lumaLocalAvg = luma3;
    if (!is1Steepest)
        lumaLocalAvg = luma4;
    lumaLocalAvg *= 0.5f;

    //
    // Traverse to both sides until reaching the end or a non-edge point
    //

    // Offset to both sides along the edge
    // 0    0    0
    // <-  posB ->
    // 1    1    1
    float2 offset;
    offset.x = (!horzSpan) ? 0.0 : g_TexelSize_Screen.x;
    offset.y = (horzSpan) ? 0.0 : g_TexelSize_Screen.y;
    // Offset in the negative direction
    float2 posN = posB - offset * s_SampleDistances[0];
    // Offset in the positive direction
    float2 posP = posB + offset * s_SampleDistances[0];

    // Get the luminance values at the offset points and compute the different from the middle point's Luminance
    float lumaEndN = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posN, 0).rgb);
    float lumaEndP = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posP, 0).rgb);
    lumaEndN -= lumaLocalAvg;
    lumaEndP -= lumaLocalAvg;

    // If the luminance difference at the endpoints exceeds the local gradient, it indicates the edge has been reached
    bool doneN = abs(lumaEndN) >= gradientScaled;
    bool doneP = abs(lumaEndP) >= gradientScaled;
    bool doneNP = doneN && doneP;

    // If the non-edge point is not reached, continue extending along this direction
    if (!doneN)
        posN -= offset * s_SampleDistances[1];
    if (!doneP)
        posP += offset * s_SampleDistances[1];

    // Continue iterating until both sides reach an edge or a maximum number of iterations
    if (!doneNP)
    {
        [unroll]
        for (int i = 2; i < FXAA_QUALITY__PS; ++i)
        {
            if (!doneN)
                lumaEndN = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posN.xy, 0).rgb) - lumaLocalAvg;
            if (!doneP)
                lumaEndP = LinearRGBToLuminance(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posP.xy, 0).rgb) - lumaLocalAvg;
            
            doneN = abs(lumaEndN) >= gradientScaled;
            doneP = abs(lumaEndP) >= gradientScaled;
            doneNP = doneN && doneP;
        
            if (!doneN)
                posN -= offset * s_SampleDistances[i];
            if (!doneP)
                posP += offset * s_SampleDistances[i];
            // Stop if both sides reach the edge
            if (doneNP)
                break;
        }
    }
    
    // Calculate the distances to the two endpoints
    float distN = horzSpan ? (posM.x - posN.x) : (posM.y - posN.y);
    float distP = horzSpan ? (posP.x - posM.x) : (posP.y - posM.y);

    // Check which endpoint is closer to the current point, take that distance
    bool directionN = distN < distP;
    float dist = min(distN, distP);
    
    // Distance between the two endpoints
    float spanLength = (distP + distN);
    
    // Pixel offset towards the nearest endpoint
    float pixelOffset = -dist / spanLength + 0.5f;
/*--------------------------------------------------------------------------*/
    
    // Check if the luminance of the current pixel is less than the average luminance of the two pixels adjacent to posB
    bool isLumaMSmaller = lumaM < lumaLocalAvg;
    
    // Determine if this is a good edge
    bool goodSpanN = (lumaEndN < 0.0) != isLumaMSmaller;
    bool goodSpanP = (lumaEndP < 0.0) != isLumaMSmaller;
    bool goodSpan = directionN ? goodSpanN : goodSpanP;
    
    // If not, do not offset
    float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
/*--------------------------------------------------------------------------*/
    
    // Calculate luminance variation over a 3x3 range
    //      [1  2  1]
    // 1/12 [2 -12 2]
    //      [1  2  1]
    float subpixNSWE = lumaNS + lumaWE;
    float subpixNWSWNESE = lumaNWSW + lumaNESE;
    float subpixA = (2.0 * subpixNSWE + subpixNWSWNESE) * (1.0 / 12.0) - lumaM;
    // Calculate subpixel offset based on this luminance variation
    float subpixB = saturate(abs(subpixA) * (1.0 / lumaRange));
    float subpixC = (-2.0 * subpixB + 3.0) * subpixB * subpixB;
    float subpix = subpixC * subpixC * g_QualitySubPix;
    
    // Select the largest offset
    float pixelOffsetSubpix = max(pixelOffsetGood, subpix);
/*--------------------------------------------------------------------------*/
    
    if (!horzSpan)
        posM.x += pixelOffsetSubpix * lengthSign;
    if (horzSpan)
        posM.y += pixelOffsetSubpix * lengthSign;

    // debug output
    // o_color =  float4(1.0f - 2.0f * pixelOffsetSubpix, 2.0f * pixelOffsetSubpix, 0.0f, 1.0f);

    o_color =  float4(g_TextureInput.SampleLevel(g_SamplerLinearClamp, posM, 0).xyz, lumaM);

}