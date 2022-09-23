for file in "$@";
do
	clang-format --dry-run --Werror --ferror-limit=20 $file;

	if [ $? -ne 0 ]; then
		exit 1
	fi
done
