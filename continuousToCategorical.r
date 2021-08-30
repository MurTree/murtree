library(caret)
library(Rcpp)
library(arulesCBA)

#command line parameters: 
#	the filename
#	the output file
#	is the last column is the label (otherwise it is the first column) (TRUE or FALSE)
#	has header (TRUE or FALSE)
#	method (e.g. "mdlp")

args = commandArgs(trailingOnly=TRUE)

input_file = "kekw.txt"
output_file = "monkas.txt"
is_last_column_target = TRUE
has_header = TRUE
method = "mdlp"

if (length(args) != 5){
  stop("Need exactly five parameters!", call.=TRUE)
}else{
	input_file = args[1]
	output_file = args[2]
	is_last_column_target = eval(parse(text=args[3]))
	has_header = eval(parse(text=args[4]))
	method = args[5]
}

input_file
output_file
is_last_column_target
has_header
method

data <- read.csv(input_file, header=has_header)
f <- "dummy"

if (is_last_column_target){
	colnames(data)[length(data)] <- "TARGET"
	f <- as.formula(paste(tail(names(data), 1), "~ ."))
	data[,ncol(data)] = as.factor(data[,ncol(data)])
}else{
	colnames(data)[1] <- "TARGET"
	f <- as.formula(paste(head(names(data), 1), "~ ."))
	data[,1] = as.factor(data[,1])	
}

#data$f <- as.factor(data$f)
data_discretized <- discretizeDF.supervised(f, data, method = method)
write.csv(data_discretized, file=output_file, row.names=FALSE)