typedef struct pagina {
    bool pagina_ativa;
    int pos_prox_livre;
    int posPagina;
    int qtd_chaves_atuais;
    bool ehFolha;
    int posFilhos[ORDEM + 1];
    int posProximo;
    int posRegistro[ORDEM];
    void* chaves[ORDEM]; 
} Pagina;