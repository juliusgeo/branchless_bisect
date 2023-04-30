set -ex
find . | grep -E "(__pycache__|\.pyc|\.egg-info|\.pyo$)" | xargs rm -rf
rm -r build
python -m pip install --ignore-installed -e .