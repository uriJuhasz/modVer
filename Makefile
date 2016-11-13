.PHONY: clean All

All:
	@echo "----------Building project:[ modVer - Debug ]----------"
	@"$(MAKE)" -f  "modVer.mk"
clean:
	@echo "----------Cleaning project:[ modVer - Debug ]----------"
	@"$(MAKE)" -f  "modVer.mk" clean
