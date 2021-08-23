##
##  Department of Electrical, Electronic and Computer Engineering.
##  EPR400/2 First Semester Report - Makefile.
##  Copyright © 2011-2021 University of Pretoria.
##

all: firstsemreport.pdf

firstsemreport.pdf: *.tex *.bib epr400.cls
	pdflatex firstsemreport
	bibtex   firstsemreport
	pdflatex firstsemreport
	pdflatex firstsemreport

clean:
	del -f *.aux *.log *.bbl *.blg *.toc *.synctex.gz *.fls *.fdb_latexmk
	del -f *converted-to.pdf firstsemreport.pdf

## End of File.
