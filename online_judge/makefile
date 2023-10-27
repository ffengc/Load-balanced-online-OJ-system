.PHONY:all
all:
	@cd compile_server;\
	make;\
	cd ..;\
	cd oj_server;\
	make;\
	cd ..;\


.PHONY:make_output
make_output:
	mkdir -p make_output/compile_server;\
	mkdir -p make_output/oj_server;\
	cp -rf ./compile_server/compile_server make_output/compile_server/;\
	cp -rf ./compile_server/temp make_output/compile_server/;\
	cp -rf ./oj_server/conf make_output/oj_server/;\
	cp -rf ./oj_server/questions make_output/oj_server/;\
	cp -rf ./oj_server/template_html make_output/oj_server/;\
	cp -rf ./oj_server/wwwroot make_output/oj_server/;\
	cp -rf ./oj_server/oj_server make_output/oj_server/;\


.PHONY:clean
clean:
	@cd compile_server;\
	make clean;\
	cd ..;\
	cd oj_server;\
	make clean;\
	cd ..;\
	rm -rf make_output