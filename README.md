# README #

The code in the repository implements the MurTree algorithm for constructing optimal classification trees, i.e., trees that minimise the misclassification score of the given dataset while respecting constraints on the depth and number of feature nodes. The sparse objective, that takes penalises each node added in the tree, is also supported. 

Details about the algorithm may be found in the paper:

"MurTree: Optimal Decision Trees via Dynamic Programming and Search" by Emir Demirović, Anna Lukina, Emmanuel Hebrard, Jeffrey Chan, James Bailey, Christopher Leckie, Kotagiri Ramamohanarao, and Peter J. Stuckey, Journal of Machine Learning Research (JMLR), 2021.

### What does the algorithm do? ###

Given a dataset where all features are binary, a maximum depth d, and the maximum number of feature nodes, the algorithm computes the decision tree that minimises the number of misclassifications on the dataset. 

The sparse objective is also supported. In this case, the objective opimised is:

misclassifications/num_instances + \alpha * num_nodes, 

where \alpha is the sparse coefficient. By default \alpha is set to zero.

The algorithm exhaustively explores the entire search space, guaranteeing that the end result is optimal.

In case your features are not binary, they can be binarised in different ways, e.g., using the script provided in the repository (continuousToCategorical.r).

### How do I compile the code? ###

Windows users can use the project files given for Visual Studio in the code.

Linux users can use the provided makefile. Go to into the folder "LinuxRelease" and type "make all". The executable "MurTree" will appear in the same folder.

### How do I run the code? ###

After compiling you should have an executable of the program. Typically use of the code would look as follows:

MurTree.exe -file anneal.txt -max-depth 4 -max-num-nodes 15 -time 600

The above command calls the MurTree algorithm to compute the tree that minimises the classification score on the dataset given in the file 'anneal.txt' using a time limit of 600 seconds. The tree can use at most 15 features nodes and have at most depth 4 in this example.

The sparse objective is also supported. The default value of the sparse coefficient is zero, but a nonzero value may be set using the parameter '-sparse-coefficient 0.1' for instance.

For other parameters of the algorithm, check DefineParameters() in main.cpp.

The output is (for now) text on the console printing the time taken, misclassification score, and some other information. Should you like to do something with the resulting tree (e.g., print it to a file), you can find the line in the code in main.cpp "MurTree::SolverResult mur_tree_solver_result = mur_tree_solver.Solve(parameters)", and the mur_tree_solver_result object contains the resulting tree. You can then traverse it as you would expect with trees in C++. In case you need help please reach out at e.demirovic@tudelft.nl.

As a sanity test, after you compile the code, run the following instances and check the time it takes to generate the tree. The values should be similar to the following:

* anneal.txt: 0.1 secs (91 misclassifications)
* australian-credit.txt: 0.87 secs (56 misclassifications)
* german-credit.txt: 1.7 secs (204 misclassifications)
* vehicle.txt: 6 secs (12 misclassifications)

The above uses -max-depth 4 and -max-num-nodes 15. The instances may be found in the folder datasetsDL. If the runtime greatly differs, either your machine is a lot slower, or (more likely) there is some problem with the compilation (please report if so).

### What is the input file format? ###

* All features zero-one. Labels are integers starting from zero. Non-binarised instances must be converted into binary form, e.g., using our binarisation script (continuousToCategorical.r).
* Each line is one instance. The first number is the label while the rest are features.
* Datasets are provided in the repository.

### Important trivia about the algorithm ###

* The resulting tree, if the algorithm terminated within the given time limit, is optimal, i.e., there is no other tree with a better objective value. This is because the algorithm exhaustively explores the search space, implicitly considering every possible decision tree within the constraints.

* The algorithm returns one possible optimal tree, but there may be many other trees that have the same objective value.

* The algorithm has several parameters that may be tweaked to improve the performance. The default configuration seemed to work best in our experiments overall, but for some instances other values might work better. Things to try out is to disable similarity-based lower bounding (-similarity-lower-bound 0), change the node selection strategy (-node-selection post-order), use the Gini coefficient for ordering features (-feature-ordering gini), use branching cache instead (-cache-type branch), and provide a good upper bound (-upper-bound 100). 

* Upon completion, the algorithm also checks whether the resulting tree has the reported misclassification score. If this check fails, an asserting is triggered which crashed the program, meaning something unexpected happened. If you encounter such a situation please report it to e.demirovic@tudelft.nl.

* The runtime of the algorithm depends on a number of factors:

	+ It is exponential with respect to the depth. Typically the algorithm terminates within a second for depths 2, 3, and for some datasets also for depth 4. However depths 5, 6, and so on may result in long runtimes. 

	+ The number of feature nodes is also important. Small values are typically easy, as well as large values, where a large value is one close to 2^d-1, where d is the depth. The most difficult problem is when the number of feature nodes is to (2^d-1)/2. This is due to the fact that the search space is the larger.

	+ The algorithm is linear with respect to the size of the dataset.

	+ The number of binary features influences the runtime in a somewhat unpredicatable way. In some cases datasets with 300 features may terminate faster than 100 features. However overall the trend is that the more binary features the dataset has, the longer the runtime. In our experiments the number of features went to approximately 1000. When binarising a dataset, one has to take into account to generate meaningful binary features, but not too many of them.

	+ One of the limitations of the algorithm is that it only works with binary features. Note that conventional decision tree algorithms implicitly binarise features at runtime. For instance, the test 'feature >= threshold' is effectively a binary feature. In case of the MurTree, these types of predicates need to be defined upfront.

	+ Setting the sparse coefficient to a nonzero value will likely result in very small trees. The runtime varies depending on the value of the sparse coefficient.

* In case you need to optimise a nonlinear objective for a binary classification problem, have a look at our paper: Demirović, Emir; Peter J. Stuckey. "Optimal decision trees for nonlinear metrics.", AAAI'21.

### Where do the instances come from? ###

We collected benchmarks from other papers, binarising some of the datasets using our script (continuousToCategorical.r). The papers are as follows:

* datasetsDL: Aglin, Gaël; Siegfried Nijssen; Pierre Schaus. "Learning optimal decision trees using caching branch-and-bound search." AAAI'20.

* datasetsNL: Verwer, Sicco; Yingqian Zhang. "Learning optimal classification trees using a binary linear program formulation." AAAI'19.

* datasetsNina: Narodytska, Nina; Ignatiev, Alexey; Pereira, Filipe; Marques-Silva, Joao. "Learning Optimal Decision Trees with SAT." IJCAI'18.

* datasetsHu: Hu, Xiyang; Cynthia Rudin; Margo Seltzer. "Optimal sparse decision trees.", NeurIPS'19.

### Contact Person ###

If you have any issues or questions about the code, please do not hestitate to reach out to Dr Emir Demirović at e.demirovic@tudelft.nl. We would also be happy to help out to incorporate our code within your system, use it in some other way than that given here, or any other feature request.