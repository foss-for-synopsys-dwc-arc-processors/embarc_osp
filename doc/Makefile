all:doc

doc:
	@echo "++Generate embARC document"
	doxygen embARC.doxyfile
	@echo "++Copy required files into generated embARC document"
	-cp -Rf documents/pic embARC_Document/html
	-rm -rf embARC_Document/html/pic/images

clean:
	@echo "++Clean generated embARC document"
	@rm -rf embARC_Document