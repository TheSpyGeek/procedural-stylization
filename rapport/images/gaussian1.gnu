
set encoding utf8
set term png
set output "gaussian1.png"

# function
#1./(sigma*sqrt(2*pi))
Gauss(x,mu,sigma) = exp( -(x-mu)**2 / (2*sigma**2) )
d1(x) = Gauss(x, 5, 1)

set xrange [0:10]
set yrange [0:1.5]

plot d1(x) with lines linewidth 1.5
