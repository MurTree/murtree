#include "parameters.h"

namespace MurTree {

    ParameterHandler DefineParameters() {
        
        ParameterHandler parameters;
        parameters.DefineNewCategory("Main Parameters");
        parameters.DefineNewCategory("Algorithmic Parameters");
        parameters.DefineNewCategory("Tuning Parameters");

        parameters.DefineStringParameter
        (
            "file",
            "Location to the dataset.",
            "", //default value
            "Main Parameters"
        );

        parameters.DefineFloatParameter
        (
            "time",
            "Maximum runtime given in seconds.",
            600, //default value
            "Main Parameters",
            0, //min value
            INT32_MAX //max value
        );

        parameters.DefineIntegerParameter
        (
            "max-depth",
            "Maximum allowed depth of the tree, where the depth is defined as the largest number of *decision/feature nodes* from the root to any leaf. Depth greater than four is usually time consuming.",
            3, //default value
            "Main Parameters",
            0, //min value
            20 //max value
        );

        parameters.DefineIntegerParameter
        (
            "max-num-nodes",
            "Maximum number of *decision/feature nodes* allowed. Note that a tree with k feature nodes has k+1 leaf nodes.",
            7, //default value
            "Main Parameters",
            0,
            INT32_MAX
        );

        parameters.DefineFloatParameter
        (
            "sparse-coefficient",
            "Assigns the penalty for using decision/feature nodes. Large sparse coefficients will result in smaller trees.",
            0.0,
            "Main Parameters",
            0.0,
            1.0
        );

        parameters.DefineBooleanParameter
        (
            "verbose",
            "Determines if the solver should print logging information to the standard output.",
            true,
            "Main Parameters"
        );

        parameters.DefineBooleanParameter
        (
            "all-trees",
            "Instructs the algorithm to compute trees using all allowed combinations of max-depth and max-num-nodes. Used to stress-test the algorithm.",
            false,
            "Main Parameters"
        );

        parameters.DefineStringParameter
        (
            "result-file",
            "The results of the algorithm are printed in the provided file, using for simple benchmarking. The output file contains the runtime, misclassification score, and number of cache entries. Leave blank to avoid printing.",
            "", //default value
            "Main Parameters"
        );

        //Internal algorithmic parameters-----------

        parameters.DefineBooleanParameter
        (
            "incremental-frequency",
            "Activate incremental frequency computation, which takes into account previously computed trees when recomputing the frequency. In our experiments proved to be effective on all datasets.",
            true,
            "Algorithmic Parameters"
        );

        parameters.DefineBooleanParameter
        (
            "similarity-lower-bound",
            "Activate similarity-based lower bounding. Disabling this option may be better for some benchmarks, but on most of our tested datasets keeping this on was beneficial.",
            true,
            "Algorithmic Parameters"
        );

        parameters.DefineStringParameter
        (
            "node-selection",
            "Node selection strategy used to decide on whether the algorithm should examine the left or right child node first.",
            "dynamic", //default value
            "Algorithmic Parameters",
            { "dynamic", "post-order" }
        );

        parameters.DefineStringParameter
        (
            "feature-ordering",
            "Feature ordering strategy used to determine the order in which features will be inspected in each node.",
            "in-order", //default value
            "Algorithmic Parameters",
            { "in-order", "random", "gini" }
        );

        parameters.DefineIntegerParameter
        (
            "random-seed",
            "Random seed used only if the feature-ordering is set to random. A seed of -1 assings the seed based on the current time.",
            3,
            "Algorithmic Parameters",
            -1,
            INT32_MAX
        );

        parameters.DefineStringParameter
        (
            "cache-type",
            "Cache type used to store computed subtrees. \"Dataset\" is more powerful than \"branch\" but may required more computational time. Need to be determined experimentally. \"Closure\" is experimental and typically slower than other options.",
            "dataset", //default value
            "Algorithmic Parameters",
            { "branch", "dataset", "closure" }
        );

        parameters.DefineIntegerParameter
        (
            "duplicate-factor",
            "Duplicates the instances the given amount of times. Used for stress-testing the algorithm, not a practical parameter.",
            1,
            "Algorithmic Parameters",
            1,
            INT32_MAX
        );

        parameters.DefineIntegerParameter
        (
            "upper-bound",
            "Initial upper bound.",
            INT32_MAX, //default value
            "Algorithmic Parameters",
            0,
            INT32_MAX
        );

        //Tuning parameters

        parameters.DefineBooleanParameter
        (
            "hyper-parameter-tuning",
            "Activate hyper-parameter tuning using max-depth and max-num-nodes as the maximum values allowed. The splits need to be provided in the appropriate folder...see the code. todo",
            false,
            "Tuning Parameters"
        );

        parameters.DefineStringParameter
        (
            "splits-location-prefix",
            "Prefix to where the splits may be found. Used in combination with hyper-parameter-tuning. The splits need to be provided in the appropriate folder...see the code. todo",
            "", //default value
            "Tuning Parameters"
        );

        parameters.DefineStringParameter
        (
            "hyper-parameter-stats-file",
            "Location of the output file that contains information about the hyper-parameter procedure.",
            "", //default value
            "Tuning Parameters"
        );

        return parameters;
    }

    void CheckParameters(ParameterHandler& parameters) {
        if (parameters.GetIntegerParameter("max-depth") > parameters.GetIntegerParameter("max-num-nodes")) {
            std::cout << "Error: The depth parameter is greater than the number of nodes!\n";
            exit(1);
        }

        if (parameters.GetIntegerParameter("max-num-nodes") > (uint32_t(1) << parameters.GetIntegerParameter("max-depth")) - 1) {
            std::cout << "Error: The number of nodes exceeds the limit imposed by the depth!\n";
            exit(1);
        }

        if (parameters.GetBooleanParameter("hyper-parameter-tuning") && parameters.GetStringParameter("splits-location-prefix") == "") {
            std::cout << "Error: hyper tuning specified but no splits given\n";
            exit(1);
        }

        if (parameters.GetBooleanParameter("hyper-parameter-tuning") && parameters.GetStringParameter("hyper-parameter-stats-file") == "") {
            std::cout << "Error: hyper tuning specified but no output file location given\n";
            exit(1);
        }
    }

}