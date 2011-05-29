
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2011 Francois Beaune
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// Interface header.
#include "constanthemisphereenvironmentedf.h"

// appleseed.renderer headers.
#include "renderer/modeling/environmentedf/environmentedf.h"
#include "renderer/modeling/input/inputarray.h"
#include "renderer/modeling/input/source.h"

// appleseed.foundation headers.
#include "foundation/math/sampling.h"

// Forward declarations.
namespace renderer  { class Project; }

using namespace foundation;
using namespace std;

namespace renderer
{

namespace
{
    //
    // Per-hemisphere constant-emittance environment EDF.
    //

    const char* Model = "constant_hemisphere_environment_edf";

    class ConstantHemisphereEnvironmentEDF
      : public EnvironmentEDF
    {
      public:
        ConstantHemisphereEnvironmentEDF(
            const char*         name,
            const ParamArray&   params)
          : EnvironmentEDF(params)
        {
            set_name(name);

            m_inputs.declare("upper_hemi_exitance", InputFormatSpectrum);
            m_inputs.declare("lower_hemi_exitance", InputFormatSpectrum);
        }

        virtual void release()
        {
            delete this;
        }

        virtual const char* get_model() const
        {
            return Model;
        }

        virtual void on_frame_begin(const Project& project)
        {
            assert(m_inputs.source("upper_hemi_exitance"));
            assert(m_inputs.source("lower_hemi_exitance"));

            check_uniform("upper_hemi_exitance");
            check_uniform("lower_hemi_exitance");

            m_inputs.evaluate_uniforms(&m_values);
        }

        virtual void sample(
            InputEvaluator&     input_evaluator,
            const Vector2d&     s,
            Vector3d&           outgoing,
            Spectrum&           value,
            double&             probability) const
        {
            outgoing = sample_sphere_uniform(s);
            value =
                outgoing.y >= 0.0
                    ? m_values.m_upper_hemi_exitance
                    : m_values.m_lower_hemi_exitance;
            probability = 1.0 / (4.0 * Pi);
        }

        virtual void evaluate(
            InputEvaluator&     input_evaluator,
            const Vector3d&     outgoing,
            Spectrum&           value) const
        {
            assert(is_normalized(outgoing));
            value =
                outgoing.y >= 0.0
                    ? m_values.m_upper_hemi_exitance
                    : m_values.m_lower_hemi_exitance;
        }

        virtual void evaluate(
            InputEvaluator&     input_evaluator,
            const Vector3d&     outgoing,
            Spectrum&           value,
            double&             probability) const
        {
            assert(is_normalized(outgoing));
            value =
                outgoing.y >= 0.0
                    ? m_values.m_upper_hemi_exitance
                    : m_values.m_lower_hemi_exitance;
            probability = 1.0 / (4.0 * Pi);
        }

        virtual double evaluate_pdf(
            InputEvaluator&     input_evaluator,
            const Vector3d&     outgoing) const
        {
            assert(is_normalized(outgoing));
            return 1.0 / (4.0 * Pi);
        }

      private:
        struct InputValues
        {
            Spectrum    m_upper_hemi_exitance;
            Alpha       m_upper_hemi_exitance_alpha;    // unused
            Spectrum    m_lower_hemi_exitance;
            Alpha       m_lower_hemi_exitance_alpha;    // unused
        };

        InputValues     m_values;
    };
}


//
// ConstantHemisphereEnvironmentEDFFactory class implementation.
//

const char* ConstantHemisphereEnvironmentEDFFactory::get_model() const
{
    return Model;
}

const char* ConstantHemisphereEnvironmentEDFFactory::get_human_readable_model() const
{
    return "Per-Hemisphere Constant Environment EDF";
}

DictionaryArray ConstantHemisphereEnvironmentEDFFactory::get_widget_definitions() const
{
    Dictionary entity_types;
    entity_types.insert("color", "Colors");
    entity_types.insert("texture_instance", "Textures");

    DictionaryArray definitions;

    definitions.push_back(
        Dictionary()
            .insert("name", "upper_hemi_exitance")
            .insert("label", "Upper Hemisphere Exitance")
            .insert("widget", "entity_picker")
            .insert("entity_types", entity_types)
            .insert("use", "required")
            .insert("default", ""));

    definitions.push_back(
        Dictionary()
            .insert("name", "lower_hemi_exitance")
            .insert("label", "Lower Hemisphere Exitance")
            .insert("widget", "entity_picker")
            .insert("entity_types", entity_types)
            .insert("use", "required")
            .insert("default", ""));

    return definitions;
}

auto_release_ptr<EnvironmentEDF> ConstantHemisphereEnvironmentEDFFactory::create(
    const char*         name,
    const ParamArray&   params) const
{
    return
        auto_release_ptr<EnvironmentEDF>(
            new ConstantHemisphereEnvironmentEDF(name, params));
}

}   // namespace renderer