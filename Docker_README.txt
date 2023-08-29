From the BEM_EMI_model folder run:

docker build -t bemi .
docker create --name bemi_container -ti -v "$(pwd)/results":/bem_emi/results bemi
docker start bemi_container
docker exec -ti bemi_container ./main --help
docker stop bemi_container