void my_roounfold_1d(string src_iter_chain = "0", int niter = 1)
{
    TFile* fdata = new TFile(Form("output/my_unfolded_spectra_%s.root", src_iter_chain.c_str()));
    TH1D* hData = (TH1D*)fdata->Get(Form("h%s", src_iter_chain.c_str()));
    RooUnfoldResponse* mRes = (RooUnfoldResponse*)fdata->Get("response_matrix");
    
    RooUnfoldBayes unfold(mRes, hData, niter);
    TH1* hUnfold = unfold.Hreco();
    hData->Draw();
    hUnfold->Draw("same");
    // TMatrixD x = unfold.UnfoldingMatrix();
    // cout << x[1][1] << endl;
    cout << unfold.PriorDistribution()[4] << endl;
}